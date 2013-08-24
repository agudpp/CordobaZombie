/*
 * GraphColoringHelper.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include "GraphColoringHelper.h"

#include <list>
#include <algorithm>

#include <debug/DebugUtil.h>


/* Code refactorized from:
 * Helper to Graph Coloring Problem
 * using Recursive-Largest-First algorithm by F. T. Leighton
 * from http://www.codeproject.com/KB/recipes/graph_coloring_using_RLF.aspx
 */
namespace {

typedef std::vector<int> IntVec;
typedef core::GraphColoringHelper::AdjacencyMatrix Matrix;

// initializing function
//
void
Init(IntVec& color,
     IntVec& degree,
     const Matrix& a,
     int &NNCount,
     int &unprocessed,
     IntVec& NN)
{
    ASSERT(color.size() == degree.size());
    ASSERT(degree.size() == NN.size());
    ASSERT(color.size() == a.getNumNodes());

    for (core::size_t i = 0, size = color.size(); i < size; ++i) {
        color[i] = 0; // be sure that at first, no vertex is colored
        degree[i] = 0; // count the degree of each vertex
        NN[i] = 0;
        for (core::size_t j = 0; j < size; ++j) {
            if (a.get(i, j) == true) {// if i-th vertex is adjacent to another
                ++(degree[i]); // increase its degree by 1
            }
        }
    }
    NNCount = 0; // number of element in NN set
    unprocessed = color.size();
}

// this function finds the unprocessed vertex of which degree is maximum
//
int
MaxDegreeVertex(const IntVec& color, const IntVec& degree)
{
    int max = -1;
    int max_i;
    const core::size_t n = color.size();

    for (core::size_t i =0; i < n; ++i) {
        if (color[i] == 0) {
            if (degree[i] > max) {
                max = degree[i];
                max_i = i;
            }
        }
    }
    return max_i;
}

// this function is for UpdateNN function
// it reset the value of scanned array
//
void
scannedInit(IntVec& scanned)
{
    const core::size_t n = scanned.size();

    for (core::size_t i = 0; i < n; ++i) {
        scanned[i] = 0;
    }
}

// this function updates NN array
//
void
UpdateNN(int ColorNumber,
         int &NNCount,
         IntVec& color,
         const Matrix& a,
         IntVec& NN)
{
    const core::size_t n = color.size();

    NNCount = 0;
    // firstly, add all the uncolored vertices into NN set
    for (core::size_t i = 0; i < n; ++i) {
        if (color[i] == 0) {
            NN[NNCount] = i;
            ++NNCount; // when we add a vertex, increase the NNCount
        }
    }

    // then, remove all the vertices in NN that
    // is adjacent to the vertices colored ColorNumber
    for (core::size_t i = 0; i < n; ++i) {
        if (color[i] == ColorNumber) { // find one vertex colored ColorNumber
            for (core::size_t j = 0; j < NNCount; ++j) {
                while (a.get(i, NN[j]) == true && NNCount > 0) {
                    // remove vertex that adjacent to the found vertex
                    NN[j] = NN[NNCount - 1];
                    --NNCount; // decrease the NNCount
                }
            }
        }
    }
}

// this function will find suitable y from NN
//
int
FindSuitableY(int ColorNumber,
              int& VerticesInCommon,
              int &NNCount,
              const IntVec& NN,
              const IntVec& color,
              const Matrix& a)
{
    const core::size_t n = color.size();
    int temp,tmp_y,y;

    // array scanned stores uncolored vertices
    // except the vertex is being processing
    IntVec scanned;
    scanned.resize(n);

    VerticesInCommon = 0;
    for (core::size_t i = 0; i < NNCount; ++i) {// check the i-th vertex in NN
        // tmp_y is the vertex we are processing
        tmp_y = NN[i];
        // temp is the neighbors in common of tmp_y
        // and the vertices colored ColorNumber
        temp = 0;
        scannedInit(scanned);
        //reset scanned array in order to check all
        //the vertices if they are adjacent to i-th vertex in NN
        for (core::size_t x = 0; x < n; ++x) {
            if (color[x] == ColorNumber) { // find one vertex colored ColorNumber
                for (core::size_t k = 0; k < n; ++k) {
                    if (color[k] == 0 && scanned[k] == 0) {
                        if (a.get(x, k) == true && a.get(tmp_y, k) == true) {
                            // if k is a neighbor in common of x and tmp_y
                            ++temp;
                            scanned[k] = 1; // k is scanned
                        }
                    }
                }
            }
        }
        if (temp > VerticesInCommon) {
            VerticesInCommon = temp;
            y = tmp_y;
        }
    }
    return y;
}

// find the vertex in NN of which degree is maximum
//
int
MaxDegreeInNN(int &NNCount,
              const IntVec& color,
              const Matrix& a,
              const IntVec& NN)
{
    const core::size_t n = color.size();
    int tmp_y; // the vertex has the current maximum degree
    int temp, max = 0;

    for (core::size_t i = 0; i < NNCount; ++i) {
        temp = 0;
        for (int j=0; j < n; j++) {
            if (color[j] == 0 && a.get(NN[i], j) == true) {
                temp ++;
            }
        }
        if (temp>max) { // if the degree of vertex NN[i] is higher than tmp_y's one
            max = temp; // assignment NN[i] to tmp_y
            tmp_y = NN[i];
        }
    }
    if (max == 0) {// so all the vertices have degree 0
        return NN[0];
    }

    // exist a maximum, return it
    return tmp_y;
}

// print out the output
//
bool
PrintOutput(const IntVec& color)
{
    const core::size_t n = color.size();
    for (core::size_t i = 0; i < n; ++i) {
        // element i-th of array color stores the color of (i+1)-th vertex
        std::cout << "Vertex " << i+1
             << " is colored " << color[i] << std::endl;
    }

    return true;
}

void
Coloring(IntVec& color,
         const IntVec& degree,
         int &unprocessed,
         int &NNCount,
         IntVec& NN,
         const Matrix& a)
{
    const core::size_t n = color.size();
    int x,y;
    int ColorNumber = 0;
    int VerticesInCommon = 0;
    while (unprocessed>0) { // while there is an uncolored vertex
        x = MaxDegreeVertex(color, degree); // find the one with maximum degree
        ++ColorNumber;
        color[x] = ColorNumber; // give it a new color
        unprocessed --;
        UpdateNN(ColorNumber, NNCount, color, a, NN); // find the set of non-neighbors of x
        while (NNCount>0) {
            // find y, the vertex has the maximum neighbors in common with x
            // VerticesInCommon is this maximum number
            y = FindSuitableY(ColorNumber, VerticesInCommon, NNCount, NN, color, a);
            // in case VerticesInCommon = 0
            // y is determined that the vertex with max degree in NN
            if (VerticesInCommon == 0) {
                y = MaxDegreeInNN(NNCount, color, a, NN);
            }
            // color y the same to x
            color[y] = ColorNumber;
            unprocessed --;
            UpdateNN(ColorNumber, NNCount, color, a, NN);
            // find the new set of non-neighbors of x
        }
    }
}

int
countNumberOfColours(const IntVec& colors)
{
    std::list<int> colorUsed;

    const core::size_t n = colors.size();
    for(int i = 0; i < n; ++i)
    {
        if(std::find(colorUsed.begin(), colorUsed.end(), colors[i]) == colorUsed.end()){
            colorUsed.push_back(colors[i]);
        }
    }

    return colorUsed.size();
}


int
calculateColors(IntVec& nodeColors, const Matrix& matrix)
{
    const core::size_t N = nodeColors.size();

    // array color[n] stores the colors of the vertices
    // color[i] = 0 if we 've not colored it yet
    IntVec color;
    color.resize(N);

    // array degree[n] stores the degrees of the vertices
    IntVec degree;
    degree.resize(N);

    // array NN[] stores all the vertices that is not adjacent to current vertex
    IntVec NN;
    NN.resize(N);

    // NNCount is the number of that set
    int NNCount;
    // unprocessed is the number of vertices with which we 've not worked
    int unprocessed;

    ASSERT(matrix.getNumNodes() != 0);

    Init(color, degree, matrix, NNCount, unprocessed, NN); // initialize the data : degree, color array ..
    Coloring(color, degree, unprocessed, NNCount, NN, matrix); // working function

    ASSERT(PrintOutput(color)); // print the result onto the console lines

    for (core::size_t i = 0; i < N; ++i) {
        nodeColors[i] = color[i];
    }

    return countNumberOfColours(color);
}

}

namespace core {

namespace GraphColoringHelper {


int
graphColoring(const AdjacencyMatrix& matrix, std::vector<int>& result)
{
    result.resize(matrix.getNumNodes());
    return calculateColors(result, matrix);
}


}
} /* namespace core */
