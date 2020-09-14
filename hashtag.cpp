#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
//  global variables to perform write operations to the output_file.txt
fstream outf;
bool first = true;
bool outputFile = false;

//  Below class defines the node structure and initiates the node with the default values when it is created.
class TagNode {
    public:
        //Node structure
        int degree, data;                       //Degree of the node and the Data stored in the node
        bool childCut;                          //boolean to check if a child has been already cut from this node
        string tagName;                         //The tag name associated to the node
        TagNode* leftNode;                      //pointer to the left node
        TagNode* rightNode;                     //pointer to the right node
        TagNode* child;                         //pointer to the child node
        TagNode* parent;                        //pointer to the parent node

        TagNode() {
        }
        //constructor to initiate the default values when a node is created
        TagNode(int val, string key) {
            childCut = false;
            data = val;
            tagName = key;
            degree = 0;
            leftNode = NULL;
            rightNode = NULL;
            parent = NULL;
            child = NULL;
        }                           
};

/*
    Below class contains the functions for all the fibonacci heap operations that we perform. It also contains the
    maxPointer which points to the maximum value in the heap. We initiate the empty heap by creating an object
    of this class type and later perform the operations on the input.
*/
class HashTags {
    public:
        TagNode* maxPointer;                    //Pointer to the maximum value in the heap

        HashTags() {
            maxPointer = NULL;                  //when a new object is created, maxPointer is assigned to NULL because the heap is empty.
        }

        /*
            Function to insert the given node in to a doubly linked list at respective levels. The start parameter points to the maximum value 
            node at that level and the node parameter referes to the key that has to be inserted. If the start pointer is NULL, then the heap
            is empty. So, the start pointer is then assigned to the given node. If the start pointer data is less than node data, it inserts 
            the given node before the start pointer node and assigns the start pointer to it. Otherwise, it inserts the given node in the 
            appropriate location in the heap. 
        */
        void insertNode(TagNode** start, TagNode* node) {
            if((*start) == NULL) {
                (*start) = node;              //assign the maxPointer to the node for the first time
                (*start)->leftNode = (*start)->rightNode = (*start);
            }
            if((*start)->data < node->data){
                //re-assign the left and right sibling pointers
                node->leftNode = (*start)->leftNode;
                (*start)->leftNode->rightNode = node;
                node->rightNode = (*start);
                (*start)->leftNode = node;
                (*start) = node;              //update the maxPointer to the new node
            }
            else {
                TagNode* it = (*start);
                while(it->rightNode != (*start) && it->rightNode->data >= node->data && it->rightNode != (*start)) {
                    it = it->rightNode;
                }
                node->rightNode = it->rightNode;
                it->rightNode->leftNode = node;
                it->rightNode = node;
                node->leftNode = it;
            } 
        }

        /*
            Function to create the new node with the given key and value. It creates a new node and calls the insertNode function to insert
            it in appropriate location and later also inserts the <key, value> pair in the hashmap.
        */
        void createNode(int value, string key, map<string, TagNode*>& hashmap) {
            TagNode* node = new TagNode(value, key);
            insertNode(&maxPointer, node);
            hashmap.insert(make_pair(key, node));       //inserts the <key, value> pair in the hashmap
        }

        /*
            Function to increase the key value if a node with the input tag already exists in the heap. If the parameter node is maxPointer,
            it increases the value by the given parameter val. If the node has a parent and it's data is less than the node data, then it
            calls the cascadeCut function to do cascade cut operation. Else-If the node data is greater than the maxPointer data, this means
            that the node is at the top level and so it inserts it before the maxPointer node. Otherwise, it inserts the node into appropriate 
            position at the top level of the heap.
        */
        void increaseKey(TagNode* node, int val) {
            node->data += val;
            if (node == maxPointer) { return; }

            if(node->parent != NULL) {
                if(node->data > node->parent->data) {
                    cascadeCut(node);                   //calls the cascadeCut function to do cascade cut operation
                }
                else {
                    //if the node has a parent and its data is greater than the node data, then re-arrange the increased node to
                    //a proper position in the child list.
                    if(node != node->parent->child && node->rightNode != node) {
                        node->leftNode->rightNode = node->rightNode;
                        node->rightNode->leftNode = node->leftNode;
                        insertNode(&(node->parent->child), node);
                    }
                }
            }
            else if (node->data > maxPointer->data) {
                if(node->leftNode != NULL) {
                    node->leftNode->rightNode = node->rightNode;
                    node->rightNode->leftNode = node->leftNode;
                }                
                if(maxPointer->leftNode != NULL) {
                    node->leftNode = maxPointer->leftNode;
                    maxPointer->leftNode->rightNode = node;
                }
                node->rightNode = maxPointer;
                maxPointer->leftNode = node;
                maxPointer = node;                    //assign the maxPointer to the node after inserting it.
            }
            else {
                node->leftNode->rightNode = node->rightNode;
                node->rightNode->leftNode = node->leftNode;  
                insertNode(&maxPointer, node);         //remove the left and right links and insert it into the heap.
            }
        }

        /*
            Below function extracts the most frequent tags. It first extracts the maxPointer and then inserts all its child into the
            heap. Then it does pairwise combine to merge all the trees of same degree and finally inserts it into the heap and updates 
            the maxPointer to the current maximum value in the heap. After writing the frequent tags into the output file, it re-inserts
            them into the heap.
        */
        void higherFrequencyTags(int n) {
            vector<TagNode*> topTags;
            int maxNow = 0;
            for(int i=0; i<n && maxPointer != NULL; i++) {
                topTags.push_back(maxPointer);
                TagNode* tempPointer = maxPointer;

                //if maxPointer is not a single node in the heap, then assign a temporary maxPointer to proceed further. Otherwise,
                //mark the maxPointer as NULL.
                if(maxPointer->rightNode != maxPointer) {
                    maxPointer = maxPointer->rightNode;
                    maxPointer->leftNode = tempPointer->leftNode;
                    tempPointer->leftNode->rightNode = maxPointer;
                }
                else {
                    maxPointer = NULL;
                    if(tempPointer->child == NULL) {
                        break;
                    }
                }

                //re-insert all the child nodes of the maxPointer into the heap at the top level.
                while(tempPointer->child != NULL) {
                    TagNode* childPointer = tempPointer->child;
                    if(tempPointer->child->rightNode != tempPointer->child) {
                        tempPointer->child = tempPointer->child->rightNode;
                    }
                    else {
                        tempPointer->child = NULL;
                    }                    
                    childPointer->parent = NULL;
                    childPointer->leftNode->rightNode = childPointer->rightNode;
                    childPointer->rightNode->leftNode = childPointer->leftNode;
                    childPointer->leftNode = childPointer->rightNode = childPointer;    

                    insertNode(&maxPointer, childPointer);
                }
                //do the pairwise combine operation
                pairWiseCombine();
            }
            //re-insert the extracted nodes back into the heap for processing the next set of inputs.
            string out = "";
            for(auto x : topTags) {
                out = out + x->tagName + ",";
                x->degree = 0;
                insertNode(&maxPointer, x);
            }

            //write the top n hashtags into the output_file.txt
            if(!out.empty()) {
                out.resize(out.size() - 1);
            }
            if(!first) {
                out = "\n" + out;
            }
            else {
                first = false;
            }
            if(outputFile) {
                outf << out;
            }
            else {
                cout << out;
            }
        }

        /*
            Function to do pairwise combine process. It maintains a degreeTable hash map to keep track of the visited degrees.
            It iterates thorugh all the trees in the heap and if a tree with same degree is found in the degreeTable, it merges
            both the trees and inserts back into the degreeTable. It repeats the process untill there is no tree found with the
            same degree in the degreeTable hashmap or it reaches the end of the heap during the iteration. Finally, it inserts
            all the trees in the degreeTable into the heap.
        */
        void pairWiseCombine() {
            map <int, TagNode*> degreeTable;
            TagNode* iterator = maxPointer;
            while(true) {
                TagNode* nextIterator = iterator->rightNode;
                iterator->leftNode = iterator->rightNode = iterator;
                //check if there is an entry in the degreeTable with same degree. If so, merge the trees of same degree.
                while(degreeTable.find(iterator->degree) != degreeTable.end()) {
                    TagNode* tableNode = degreeTable.find(iterator->degree)->second;
                    if(tableNode->data >= iterator->data) {
                        degreeTable.erase(degreeTable.find(iterator->degree));
                        makeChild(&tableNode, &iterator);   //make the current tree as a child to the tree in the degreeTable
                        iterator = tableNode;
                    }
                    else {
                        degreeTable.erase(degreeTable.find(iterator->degree));
                        makeChild(&iterator, &tableNode);   //make the tree in the degreeTable as a child to the current tree
                    }
                }
                degreeTable.insert(make_pair(iterator->degree, iterator));      //insert back the merged tree into the degreeTable
                if(nextIterator != maxPointer) {
                    iterator = nextIterator;
                }
                else {
                    break;
                }
            }
            //Newly create the heap with the trees in the degreeTable.
            maxPointer = NULL;
            for(auto x : degreeTable) {
                insertNode(&maxPointer, x.second);
            }            
        }

        /*
            This is a helper function for the pairWiseCombine(). If two nodes, parent and child are passed as input, it makes the 
            child node as a child to the parent node. This is called while doing the pairwise combine process to make a tree as a
            child to the already existing tree of same degree in the degreeTable or vice-versa.
        */
        void makeChild(TagNode** parent, TagNode** child) {        
            (*child)->parent = (*parent);
            (*child)->childCut = false;
            //increase the degree of the parent and insert the child into the parent's child doubly linked list
            (*parent)->degree = (*parent)->degree + 1;
            if((*parent)->child != NULL) {
                insertNode(&(*parent)->child, *(child));
            }
            else {
                (*parent)->child = (*child);
            }
        }

        /*
            Function to do the cascade cut operation for the given node. It first removes the node from the child list of its
            parent. Then changes the degree of its parent and re-inserts the node into the heap at the top level. This process
            repeats until there is no parent or the childCut is set to false to the parent node.
        */
        void cascadeCut(TagNode* node) {
            do {
                TagNode* par = node->parent;
                if(node == node->parent->child){
                    if(node->rightNode != node) {
                        node->parent->child = node->rightNode;
                    }
                    else {
                        node->parent->child = NULL;
                    }
                }
                node->parent = NULL;
                node->leftNode->rightNode = node->rightNode;
                node->rightNode->leftNode = node->leftNode;

                par->degree = par->degree - 1;                //re-assign the degree to its parent

                node->leftNode = node->rightNode = node;
                insertNode(&maxPointer, node);                //re-insert the extracted node into the heap at the root level
                node = par;
            }
            //repeat the cascade cut process untill the node reaches the root level or if any of the parent's childCut set to false.
            while(node->childCut && node->parent != NULL);      
            node->childCut = true;
        }
};
/*
    Main function
    It reads the data line by line from the provided input file and checks if the read hashtag is already present in the hashmap.
    If so, it calls increaseKey function to increase the nodes' value by the input value. Otherwise, it calls the createNode function
    to create a new node with the read values and to insert it into the heap and the hashmap. If the input line is just an integer,
    it calls the higherFrequencyTags function to write the top n hashtags to the output file. Finally, it stops reading from the file
    when it reads a key word 'stop'.
*/
int main(int argc, char *argv[])
{
    map<string, TagNode*> hashmap;
    HashTags tags = HashTags();
    fstream inpfile;
    string ip;
    inpfile.open(argv[1], ios::in);
    if(argc > 2) {
        outf.open(argv[2], ios::out);
        outputFile = true;
    }
    while(getline(inpfile, ip)) {
        //input line is a data about a hashtag
        if(ip[0] == '#') {
            string key, value;
            stringstream keyStream(ip);
            getline(keyStream, key, ' ');
            getline(keyStream, value, ' ');
            key = key.substr(1, key.length()-1);
            if(hashmap.find(key) != hashmap.end()) {
                if(stoi(value) > 0) {
                    tags.increaseKey(hashmap.find(key)->second, stoi(value));
                }
            }
            else {
                tags.createNode(stoi(value), key, hashmap);
            }          
        }
        //input line is an integer and so a query
        else if(isdigit(ip[0])) {
            if(hashmap.size() > 0) {
                tags.higherFrequencyTags(stoi(ip));
            }
        }
        //stops reading the file
        else if(ip == "stop") {
            break;
        }
    }
    inpfile.close();
    if(outputFile) {
        outf.close();
    }
    return 0;
}