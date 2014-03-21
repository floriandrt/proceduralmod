#ifndef TREE_H
#define TREE_H

#include <cstddef>
#include <iostream>
#include <stdlib.h>

template<class T>
class Tree{
private:
    bool isLeaf; //vrai si c'est une feuille
    T* value; //pointeur sur ce que contient un noeud
    Tree** children; //tableau de pointeurs des enfants de ce noeud
    int childrenSize;

public:
    Tree() : isLeaf(true), value(), children(NULL), childrenSize(0){}
    Tree(T* v);
    Tree(const Tree& t);
    ~Tree();
    void setValue(T* v);
    T* getValue();
    void setIsLeaf(bool b);
    bool getIsLeaf();
    void setChildren(Tree** c, int size);
    int getChildrenSize();
    Tree<T>* operator[](int ind);
    template<class S>
    friend std::ostream& operator<<(std::ostream& out, const Tree<S>& t);
    //void next(); //parcourt les enfants
};

template<class T>
Tree<T>::Tree(T* v){
    isLeaf = true;
    value = v;
    children = NULL;
    childrenSize = 0;
}

template<class T>
Tree<T>::~Tree(){
    std::cout << "destruction d'arbre, value : " << *value << std::endl;
    delete value;
    if(!isLeaf){
        for(int i = 0; i<childrenSize; i++){
//            children[i]->~Tree();
            delete(children[i]);
//            free(children[i]);
        }
        delete[] children;
    }
}

template<class T>
void Tree<T>::setValue(T *v){
    delete value;
    value = v;
}

template<class T>
T* Tree<T>::getValue(){
    return value;
}

template<class T>
void Tree<T>::setIsLeaf(bool b){
    isLeaf = b;
}

template<class T>
bool Tree<T>::getIsLeaf(){
    return isLeaf;
}

template<class T>
void Tree<T>::setChildren(Tree<T>** c, int size){
    for(int i = 0; i<childrenSize; i++){
        delete children[i];
    }
    delete[] children;
    children = c;
    childrenSize = size;
    isLeaf = false;
}

template<class T>
int Tree<T>::getChildrenSize(){
    return childrenSize;
}

template<class T>
Tree<T>* Tree<T>::operator[](int ind){
    if(ind < 0 || ind >= childrenSize){
        std::cerr << "Indice d'enfant qui ne convient pas" << std::endl;
    }
    return children[ind];
}

template<class S>
std::ostream& operator<<(std::ostream& out, const Tree<S>& t){
    out << *(t.value) << "-> ";
    if(t.isLeaf){
        out << "X" << std::endl;
    }else{
        out << std::endl << "____" << std::endl;
    }
    for(int i = 0; i<t.childrenSize; i++){
       out << *(t.children[i]);
    }
    if(!t.isLeaf){
        out << "____" << std::endl;
    }
    return out;
}


#endif // TREE_H
