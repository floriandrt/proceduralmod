#ifndef TREE_H
#define TREE_H

#include <cstddef>
#include <iostream>
#include <stdlib.h>
#include <vector>

template<class T>
class Tree{
private:
    bool isLeaf; //vrai si c'est une feuille
    T* value; //pointeur sur ce que contient un noeud
    std::vector<Tree*> children; //tableau de pointeurs des enfants de ce noeud
    std::vector<Tree*> parents; //tableau de pointeurs des parents de ce noeud
    std::vector<Tree<T>*> recentDataTree;

public:
    Tree() : isLeaf(true), value(NULL){}
    Tree(T* v);
    Tree(const Tree& t);
    ~Tree();
    void setValue(T* v);
    T* getValue();
    void setIsLeaf(bool b);
    bool getIsLeaf();
    void addParent(Tree<T>* p);
    Tree<T>* getParent(int i);
    int getParentSize();
    void setParentsVec(std::vector<Tree<T>*> p);
    std::vector<Tree<T>*>& getParentsVec();
    void clearParents();
    void addChild(Tree<T>* c);
    int getChildrenSize();
    void setChildrenVec(std::vector<Tree<T>*> c);
    std::vector<Tree<T>*>& getChildrenVec();
    void clearChildren();
    void addRecentDataTree(Tree<T>* t);
    void eraseRecentDataTree(Tree<T>* t);
    int getRecentTreeSize();
    void clearRecent();
    Tree<T>* getRecentDataTree(int i);
    std::vector<Tree<T>*>& getRecentVecTree();
    int getRecentSize();
    bool notInRecentTree(Tree<T>* t);
    Tree<T>* operator[](int ind);
    template<class S>
    friend std::ostream& operator<<(std::ostream& out, const Tree<S>& t);
};

template<class T>
Tree<T>::Tree(T* v){
    isLeaf = true;
    value = v;
}

template<class T>
Tree<T>::Tree(const Tree& t){
    isLeaf = t.isLeaf;
    value = new T(*(t.value));
    children = t.children;
    parents = t.parents;
    recentDataTree = t.recentDataTree;
}

template<class T>
Tree<T>::~Tree(){
    std::cout << "destruction d'arbre, value : " << *value << std::endl;
    delete value;
    if(!isLeaf){
        for(int i = 0; i<(int)children.size(); i++){
            delete(children[i]);
        }
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
void Tree<T>::addParent(Tree<T>* p){
    for(int i = 0; i<(int)parents.size(); i++){
        if(parents[i] == p){
            return;
        }
    }
    parents.push_back(p);
}

template<class T>
Tree<T>* Tree<T>::getParent(int i){
    return parents[i];
}

template<class T>
int Tree<T>::getParentSize(){
    return parents.size();
}

template<class T>
void Tree<T>::setParentsVec(std::vector<Tree<T>*> p){
    parents = p;
}

template<class T>
std::vector<Tree<T>*>& Tree<T>::getParentsVec(){
    return parents;
}

template<class T>
void Tree<T>::clearParents(){
    parents.clear();
}

template<class T>
void Tree<T>::addChild(Tree<T>* c){
    children.push_back(c);
    children[children.size()-1]->addParent(this);
    isLeaf = false;
}

template<class T>
int Tree<T>::getChildrenSize(){
    return children.size();
}

template<class T>
void Tree<T>::setChildrenVec(std::vector<Tree<T>*> c){
    children = c;
}

template<class T>
std::vector<Tree<T>*>& Tree<T>::getChildrenVec(){
    return children;
}

template<class T>
void Tree<T>::clearChildren(){
    children.clear();
}

template<class T>
void Tree<T>::addRecentDataTree(Tree<T>* t){
    recentDataTree.push_back(t);
}

template<class T>
void Tree<T>::eraseRecentDataTree(Tree<T>* t){
    for(int i = 0; i<(int)recentDataTree.size(); i++){
        std::cerr << "ERASE RECENT t : " << t << ", recent i : " << recentDataTree[i] << std::endl;
        if(t == recentDataTree[i]){
            recentDataTree.erase(recentDataTree.begin()+i);
            return;
        }
    }
}

template<class T>
Tree<T>* Tree<T>::getRecentDataTree(int i){
    return recentDataTree[i];
}

template<class T>
void Tree<T>::clearRecent(){
    recentDataTree.clear();
}

template<class T>
std::vector<Tree<T>*>& Tree<T>::getRecentVecTree(){
    return recentDataTree;
}

template<class T>
int Tree<T>::getRecentTreeSize(){
    return recentDataTree.size();
}

template<class T>
bool Tree<T>::notInRecentTree(Tree<T>* t){
    for(int i = 0; i<(int)recentDataTree.size(); i++){
        if(recentDataTree[i] == t){
            return false;
        }
    }
    return true;
}

template<class T>
Tree<T>* Tree<T>::operator[](int ind){
    if(ind < 0 || ind >= (int)children.size()){
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
