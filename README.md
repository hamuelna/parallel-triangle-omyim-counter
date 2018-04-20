# Omyim And Triangle Counting

* This is triangle counting base on the forward algortihm base on the following paper https://arxiv.org/abs/cs/0609116

An omyim is a 4-vertex graph structure, where vertices p, q, r, s are an omyim if p, q, r form a triangle and there is an edge between q and s. That is, an omyim is a triangle with a tail. An example is shown below:

```
p
|\
| q--s
|/ 
r

```

* This is implemented using C++ and Cilk plugin

* If you are not using linux and use a mac or window you can do `./g++.sh main.cpp -o main` to build and ./run.sh ./main to run the program

* The graph reader expect the following format you can modify the code if you do not like it and you want it in a different format

```
node_1,node_2
1,2
3,4
...
``` 
