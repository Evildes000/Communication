#include "people.h"
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <algorithm>

template<typename T>
void PrintSet(const std::set<T>& myset){
    //cbegin returns a const iterator and begin() returns a regular iterator
    //you can't change the value of a element via dereferencing a const iterator
    for(std::set<int>::iterator it = myset.cbegin(); it!=myset.cend();it++){
        std::cout << *it << std::endl;
    }
}



template<typename T1, typename T2>
//void PrintMap(const std::map<T1,T2>& mymap, typename std::map<T1,T2>::iterator it){
void PrintMap(const std::map<T1,T2>& mymap){
    //if you need use template class in a template function, add the keyword typename before statment;
    typename std::map<T1,T2>::const_iterator it = mymap.cbegin();
    while(it != mymap.cend()){
        std::cout << it->first << "=>" << it->second << std::endl;
        it++;
    }
}

int main(){
    //the elements of an associative Container are always sorted in a form of binary tree
    //each time you insert or erase a element from a associative container, the binary tree will 
    //be rearranged in a right order.Therefore push_back can't be used for associative container

    std::set<int> myset = {1,2,3};
    //Duplicated element is not allowed in set, but only in multiset
    myset.insert(2);
    std::multiset mymultiset = {1,2,2,4};
    //find where is a particular number in myset
    std::set<int>::iterator it = myset.find(2);

    //this will not assert 19 at where it points, but this can improve the performance of insert
    //user can't decide where a element should be inserted 
    myset.insert(it, 19);
    myset.erase(it);
    PrintSet(myset);
    
    //set/multiset is read only, it's not allowed to modify elements in set/multiset
    

    std::map<char,int> mymap = {{'w',1},{'s',2}};
    //two ways to insert pairs into a map container
    mymap.insert(std::make_pair('x',3));
    mymap.insert(std::pair<char,int>('D',4));

    std::map<char,int>::iterator itr = mymap.begin();
    std::cout << (*itr).first << std::endl;
    PrintMap(mymap);

    //lamda is aynsnom function. Unlike regular function, lamda doesn't have name which you can call like a regular function whenever you want 
    //use it, it is just a samll piece if code that has same functionity like a regular function.
    //the advantage of lamda is it can make your code fatser to be run and clean to be read
    //structure of a lamda looks like this: [](){} very similar as regular function
    //in [] we can put elements that we want to capture, for using it later in {}.
    //in () are the parametera need to be passed. in {} is the definition of lamda

    std::vector<int> myvec{1,2,3,4,5,6};
    int d = 19;
    //elements in capture bracktes are unchangable within {}. If you want to change them, you should pass them as reference. 
    std::for_each(myvec.begin(),myvec.end(),[&d](int x){d = 10;std::cout << x << d << std::endl;});
    std::cout << d << std::endl;





    
}