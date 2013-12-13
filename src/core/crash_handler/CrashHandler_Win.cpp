/*
 * CrashHandler.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: agustin
 */

#include "CrashHandler.h"

#define PACKAGE "CordobaZombie"

#include <windows.h>
#include <excpt.h>
#include <imagehlp.h>
#include <bfd.h>
#include <psapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <assert.h>

// logger
#include <logger/LoggerManager.h>



#define BUFFER_MAX (16*1024)

// Auxiliary methods
//
namespace {

struct bfd_ctx
{
    bfd * handle;
    asymbol ** symbol;
};

struct bfd_set
{
    char * name;
    struct bfd_ctx * bc;
    struct bfd_set *next;
};

struct find_info
{
    asymbol **symbol;
    bfd_vma counter;
    const char *file;
    const char *func;
    unsigned line;
};

struct output_buffer
{
    char * buf;
    size_t sz;
    size_t ptr;
};

static void
output_init(struct output_buffer *ob, char * buf, size_t sz)
{
    ob->buf = buf;
    ob->sz = sz;
    ob->ptr = 0;
    ob->buf[0] = '\0';
}

static void
output_print(struct output_buffer *ob, const char * format, ...)
{
    if (ob->sz == ob->ptr)
        return;
    ob->buf[ob->ptr] = '\0';
    va_list ap;
    va_start(ap, format);
    vsnprintf(ob->buf + ob->ptr, ob->sz - ob->ptr, format, ap);
    va_end(ap);

    ob->ptr = strlen(ob->buf + ob->ptr) + ob->ptr;
}

static void
lookup_section(bfd *abfd, asection *sec, void *opaque_data)
{
    struct find_info *data = (struct find_info *) opaque_data;

    if (data->func)
        return;

    if (!(bfd_get_section_flags(abfd, sec) & SEC_ALLOC))
        return;

    bfd_vma vma = bfd_get_section_vma(abfd, sec);
    if (data->counter < vma || vma + bfd_get_section_size(sec) <= data->counter)
        return;

    bfd_find_nearest_line(abfd,
                          sec,
                          data->symbol,
                          data->counter - vma,
                          &(data->file),
                          &(data->func),
                          &(data->line));
}

static void
find(struct bfd_ctx * b,
     DWORD offset,
     const char **file,
     const char **func,
     unsigned *line)
{
    struct find_info data;
    data.func = NULL;
    data.symbol = b->symbol;
    data.counter = offset;
    data.file = NULL;
    data.func = NULL;
    data.line = 0;

    bfd_map_over_sections(b->handle, &lookup_section, &data);
    if (file) {
        *file = data.file;
    }
    if (func) {
        *func = data.func;
    }
    if (line) {
        *line = data.line;
    }
}

static int
init_bfd_ctx(struct bfd_ctx *bc,
             const char * procname,
             struct output_buffer *ob)
{
    bc->handle = NULL;
    bc->symbol = NULL;

    bfd *b = bfd_openr(procname, 0);
    if (!b) {
        output_print(ob, "Failed to open bfd from (%s)\n", procname);
        return 1;
    }

    int r1 = bfd_check_format(b, bfd_object);
    int r2 = bfd_check_format_matches(b, bfd_object, NULL);
    int r3 = bfd_get_file_flags(b) & HAS_SYMS;

    if (!(r1 && r2 && r3)) {
        bfd_close(b);
        output_print(ob, "Failed to init bfd from (%s)\n", procname);
        return 1;
    }

    void *symbol_table;

    unsigned dummy = 0;
    if (bfd_read_minisymbols(b, FALSE, &symbol_table, &dummy) == 0) {
        if (bfd_read_minisymbols(b, TRUE, &symbol_table, &dummy) < 0) {
            free(symbol_table);
            bfd_close(b);
            output_print(ob, "Failed to read symbols from (%s)\n", procname);
            return 1;
        }
    }

    bc->handle = b;
    bc->symbol = (asymbol**) symbol_table;

    return 0;
}

static void
close_bfd_ctx(struct bfd_ctx *bc)
{
    if (bc) {
        if (bc->symbol) {
            free(bc->symbol);
        }
        if (bc->handle) {
            bfd_close(bc->handle);
        }
    }
}

static struct bfd_ctx *
get_bc(struct output_buffer *ob, struct bfd_set *set, const char *procname)
{
    while (set->name) {
        if (strcmp(set->name, procname) == 0) {
            return set->bc;
        }
        set = set->next;
    }
    struct bfd_ctx bc;
    if (init_bfd_ctx(&bc, procname, ob)) {
        return NULL;
    }
    set->next = (bfd_set*) calloc(1, sizeof(*set));
    set->bc = (bfd_ctx*) malloc(sizeof(struct bfd_ctx));
    memcpy(set->bc, &bc, sizeof(bc));
    set->name = strdup(procname);

    return set->bc;
}

static void
release_set(struct bfd_set *set)
{
    while (set) {
        struct bfd_set * temp = set->next;
        free(set->name);
        close_bfd_ctx(set->bc);
        free(set);
        set = temp;
    }
}

static void
_backtrace(struct output_buffer *ob,
           struct bfd_set *set,
           int depth,
           LPCONTEXT context)
{
    char procname[MAX_PATH];
    GetModuleFileNameA(NULL, procname, sizeof procname);

    struct bfd_ctx *bc = NULL;

    STACKFRAME frame;
    memset(&frame, 0, sizeof(frame));

    frame.AddrPC.Offset = context->Eip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context->Esp;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context->Ebp;
    frame.AddrFrame.Mode = AddrModeFlat;

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    char symbol_buffer[sizeof(IMAGEHLP_SYMBOL) + 255];
    char module_name_raw[MAX_PATH];

    while (StackWalk(IMAGE_FILE_MACHINE_I386,
                     process,
                     thread,
                     &frame,
                     context,
                     0,
                     SymFunctionTableAccess,
                     SymGetModuleBase,
                     0)) {

        --depth;
        if (depth < 0)
            break;

        IMAGEHLP_SYMBOL *symbol = (IMAGEHLP_SYMBOL *) symbol_buffer;
        symbol->SizeOfStruct = (sizeof *symbol) + 255;
        symbol->MaxNameLength = 254;

        DWORD module_base = SymGetModuleBase(process, frame.AddrPC.Offset);

        const char * module_name = "[unknown module]";
        if (module_base
            && GetModuleFileNameA((HINSTANCE) module_base,
                                  module_name_raw,
                                  MAX_PATH)) {
            module_name = module_name_raw;
            bc = get_bc(ob, set, module_name);
        }

        const char * file = NULL;
        const char * func = NULL;
        unsigned line = 0;

        if (bc) {
            find(bc, frame.AddrPC.Offset, &file, &func, &line);
        }

        if (file == NULL) {
            DWORD dummy = 0;
            if (SymGetSymFromAddr(process,
                                  frame.AddrPC.Offset,
                                  &dummy,
                                  symbol)) {
                file = symbol->Name;
            } else {
                file = "[unknown file]";
            }
        }
        if (func == NULL) {
            output_print(ob,
                         "0x%x : %s : %s \n",
                         frame.AddrPC.Offset,
                         module_name,
                         file);
        } else {
            output_print(ob,
                         "0x%x : %s : %s (%d) : in function (%s) \n",
                         frame.AddrPC.Offset,
                         module_name,
                         file,
                         line,
                         func);
        }
    }
}

static LPTOP_LEVEL_EXCEPTION_FILTER g_prev = NULL;
static char EXCEPTION_HEADER[1024];

static LONG WINAPI
exception_filter(LPEXCEPTION_POINTERS info)
{
    struct output_buffer ob;
    char buffer[BUFFER_MAX];
    output_init(&ob, buffer, BUFFER_MAX);

    if (!SymInitialize(GetCurrentProcess(), 0, TRUE)) {
        output_print(&ob, "Failed to init symbol context\n");
    } else {
        bfd_init();
        struct bfd_set *set = (bfd_set*) calloc(1, sizeof(*set));
        _backtrace(&ob, set, 128, info->ContextRecord);
        release_set(set);

        SymCleanup(GetCurrentProcess());
    }

    // save the data into the log manager
    //
    core::LoggerManager::log("\n\n"
        "-----------------------------------------------------------------------\n"
        "\t\t\tCRASH DETECTED!\n"
        "-----------------------------------------------------------------------\n",
        core::LogMessageStyle::LOG_MSG_STYLE_INVERT_RED);

    // if we have header, print it
    if (strlen(EXCEPTION_HEADER) > 0) {
        core::LoggerManager::log(EXCEPTION_HEADER,
                                 core::LogMessageStyle::LOG_MSG_STYLE_RED);
        // clear the exception header
        EXCEPTION_HEADER[0] = '\0';
    }

    // log the backtrace now
    core::LoggerManager::log(buffer, core::LogMessageStyle::LOG_MSG_STYLE_RED);

    exit(1);

    return 0;
}


static void
exception_thrower(int sig)
{
    // we will set the header of the exception
    strcat(EXCEPTION_HEADER, "We receive a signal of type: ");
    printf("here we are with signal: ");
    switch (sig) {
    case SIGTERM:   strcat(EXCEPTION_HEADER,"SIGTERM");     break;
    case SIGSEGV:   strcat(EXCEPTION_HEADER,"SIGSEGV");     break;
    case SIGILL:    strcat(EXCEPTION_HEADER,"SIGILL");      break;
    case SIGABRT:   strcat(EXCEPTION_HEADER,"SIGABRT");     break;
    case SIGFPE:    strcat(EXCEPTION_HEADER,"SIGFPE");      break;
    default:
    {
        char auxBuff[12];
        sprintf(auxBuff, "%d", sig);
        strcat(EXCEPTION_HEADER, "Unknown signal: ");
        strcat(EXCEPTION_HEADER, auxBuff);
    }
    }
    strcat(EXCEPTION_HEADER, "\n\n");

    // ugly way to call the real exception handler in microsoft, probably there
    // are a lot of other betters ways to do it!
    // TODO: improve this crap.
    //
    int *p = 0;
    *p +=1;
}

}

namespace core {

// Windows implementation

CrashHandler::CrashHandler()
{

}

CrashHandler::~CrashHandler()
{
    // set the previous exception handler... just in case
    SetUnhandledExceptionFilter(g_prev);
    g_prev = NULL;
}


bool
CrashHandler::configureSignals(void)
{
    if (g_prev == NULL) {
        // set the exception handler and the signals to be caught
        //
        g_prev = SetUnhandledExceptionFilter(exception_filter);
        signal(SIGTERM, exception_thrower);
        signal(SIGSEGV, exception_thrower);
        signal(SIGILL, exception_thrower);
        signal(SIGABRT, exception_thrower);
        signal(SIGFPE, exception_thrower);

        // reset the EXCEPTION_HEADER
        EXCEPTION_HEADER[0] = '\0';
    }
}

} /* namespace core */
