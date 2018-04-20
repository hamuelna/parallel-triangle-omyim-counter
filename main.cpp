#include <iostream>
#include <cilk/cilk.h>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <utility>
#include <mutex>
#include <list>
#include <atomic>
#include <mutex>
#include <set>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;



void print_graph(unordered_map<int, unordered_set<int>> G){
    for(auto p: G){
        cout << p.first << ": ";
        for(auto elt: p.second){
            cout << elt << " ";
        }
        cout << endl;
    }
}

/*
intersect between two set 
*/
unordered_set<int> intersect(unordered_set<int> a, unordered_set<int> b){
    unordered_set<int> result;
    for(auto elt: a){
        auto itr = b.find(elt);
        if (itr != b.end()){
            result.emplace(elt);
        }
    }
    return result;
}


int main() {
    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    unordered_map<int, unordered_set<int>> G;
    unordered_map<int, unordered_set<int>> DG;
    //take in a graph from a file
    ifstream gin("artist_edges.csv");
    const char delim = ',';
    const int skipped = 0; 
    //create a graph
    //as you can see c++ code is sometime ugly
    string line;
    int skipCounter = 0;
    while (getline(gin, line)){
        if (skipCounter > skipped){
            istringstream lineStream(line);
            string su,sv;
            getline(lineStream, su, delim);
            getline(lineStream, sv, delim);
            int u = stoi(su) ,v = stoi(sv);
            unordered_map<int, unordered_set<int>>::const_iterator uot = G.find(u);
            if(uot == G.end()){
                unordered_set<int> set;
                set.emplace(v);
                G.emplace(u, set);
            }else {
                G.at(u).emplace(v);
            }
            unordered_map<int, unordered_set<int>>::const_iterator vot = G.find(v);
            if (vot == G.end()){
                unordered_set<int> set;
                set.emplace(u);
                G.emplace(v, set);
            }else{
                G.at(v).emplace(u);
            }
        }
        skipCounter++;

    }
    gin.close();

    DG = G;

    
    unordered_map<int, int> D;
    //calcuate deg of graph
    for(auto p: G){
        D.emplace(p.first, p.second.size());
    }

    

    /*
    change graph from undirected to a directed graph where (u,v) 
    the smaller degree vertex point to the vertex with the bigger
    degree
    */
   for(auto p: G){
       int u = p.first;
       auto unbrs = p.second;
       int degu = D.at(u);
       for(auto v: unbrs){
           int degv = D.at(v);
           if (degv > degu){
               DG.at(v).erase(u);
           }else if(degv == degu){ //handle cases where deg are the same use node number
               if(v > u){
                   DG.at(v).erase(u);
               }
           }
       }
   }

   /* do some hack to use cilk_for at the cost of using more space */

   vector<int> V;
   for(auto p: G){
       V.push_back(p.first);
   }



   /* main algorithm */

   char map[] = {'!', 'a', 'b', 'c', 'd', 'e'};
   atomic<long> ntri = {0}; //use atomic for parallelization
   atomic<long> nomyim = {0};
   cilk_for(int i = 0; i < V.size(); i++){
       int u = V.at(i);
       auto unbrs = DG.at(u);
       for(auto v: unbrs){
           auto vnbrs = DG.at(v);
           auto sect = intersect(unbrs, vnbrs);
           ntri += sect.size();
           for (auto w: sect){
               //this calculation come from how every vertex will be connected
               //in a triangle for instance there will be at least 2 edge in each node
               //since we are calculating for 3 vertex we substract 2x3 = 6
               //to eliminate the triangle edge now we will be left with a
               //an edge that is not connected to a triangle which is the
               //edge that we wanted
               nomyim += (D.at(u) + D.at(v) + D.at(w)) - 6;
           }
       }
   }
   chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
   chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
   cout << "ntri " << ntri << endl;
   cout << "n omyim " << nomyim << endl;
   cout << "time taken " << time_span.count() << " s" << endl;

   return 0;
}
