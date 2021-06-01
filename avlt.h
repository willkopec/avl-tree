
#pragma once

#include <iostream>
#include <stack>
#include <algorithm>
#include <cmath>

using namespace std;

template <typename KeyT, typename ValueT>
class avlt
{
private:
  struct NODE
  {
    KeyT   Key;
    ValueT Value;
    NODE*  Left;
    NODE*  Right;
    bool   isThreaded;
    int    Height;
  };

  NODE* Root;  // pointer to root node of tree (nullptr if empty)
  int   Size = 0;  // # of nodes in the tree (0 if empty)
  NODE* Next;  // used by threaded inorder traversal to denote next key

  // helper functions that return the left/right nodes of cur node
  NODE* _getActualLeft(NODE* cur) const
  {
    return cur->Left;
  }

  NODE* _getActualRight(NODE* cur) const
  {
    if (cur->isThreaded)  // then actual Right ptr is null:
      return nullptr;
    else  // actual Right is contents of Right ptr:
      return cur->Right;
  }


  //
  // _free
  // 
  // frees each node of the tree in post-order
  //
  void _free(NODE* cur)
  {
    if (cur == nullptr)
      return;
    else
    {
      _free(_getActualLeft(cur));
      _free(_getActualRight(cur));
      delete cur;
    }
  }

  //
  // _copy
  //
  // copy constructor/= operator helper function - recursive copy tree function that copys all left and right nodes recursively
  void _copy(NODE* &cur, NODE* other)
  {
    if(other == nullptr){ 
            return;
    } //if other is null, return
        NODE *node = new NODE(); //create new node
        node->Key = other->Key; //copy all info of current other node and set it into the new node
        node->Value = other->Value;
        node->Height = other->Height;
        node->isThreaded = other->isThreaded;
        node->Left = other->Left;
        node->Right = other->Right;
        cur = node; //after all node info is copied into new node, set it to cur and return

        _copy(node->Left,other->Left); //then recursively go left node
		
        if(other->isThreaded == false){ //if the tree is not threaded and has right node, go right, continue...
            _copy(node->Right, other->Right);
        }
  }

  // 
  // _dump
  //
  void _dump(ostream& output, NODE* cur) const
  {
    if (cur == nullptr)
      return; //if the avlt is empty, return, nothing to dump
    else
    { //else recursively dump
      _dump(output, _getActualLeft(cur)); // recursively dump left

      if(cur->isThreaded == true && cur->Right != nullptr){ //dump right threaded node if its threaded
              output <<"(" << cur->Key << "," << cur->Value << "," << cur->Height << "," << cur->Right->Key  << ")" << endl;
              cur->Right = NULL;
          }else{
              output <<"(" << cur->Key << "," << cur->Value << "," << cur->Height << ")" << endl;
          }
          
      _dump(output, _getActualRight(cur)); //recursively dump right
    }
  }
 

public:
  // default constructor:
  // Creates an empty tree.
  avlt()
  {
    Root = nullptr;
    Size = 0;
    Next = nullptr;
  }

  // copy constructor
  avlt(const avlt& other)
  {
    Root = nullptr;
        Size = other.Size;
        Next = nullptr;

        _copy(Root, other.Root);  // to be safe, copy this state as well:
  }

  // destructor:
  // Called automatically by system when tree is about to be destroyed;
  // this is our last chance to free any resources / memory used by
  // this tree.
  //
  virtual ~avlt()
  {
    _free(Root);
  }

  // operator=
  // Clears "this" tree and then makes a copy of the "other" tree.
  //
  avlt& operator=(const avlt& other)
  {
    //clear the current tree
    clear();
    //and copy the other one
    _copy(Root, other.Root);
    Size = other.Size;

    this->Next = other.Next;  // to be safe, copy this state as well:

    return *this;
  }

  // clear:
  // Clears the contents of the tree, resetting the tree to empty.
  void clear()
  {
    _free(Root);

    // now reset to empty:
    Root = nullptr;
    Size = 0;
    Next = nullptr;
  }

  // size:
  // Returns the # of nodes in the tree, 0 if empty.
  //
  // Time complexity:  O(1) 
  int size() const
  {
    return Size;
  }

  // 
  // height:
  //
  // Returns the height of the tree, -1 if empty.
  //
  // Time complexity:  O(1) 
  //
  int height() const
  {
    if (Root == nullptr) //if tree is empty, return -1
      return -1;
    else //else return the proper height of the tree
      return Root->Height;
  }
  
	// helper function, nodeHeight and heightRight - Returns the node height of the current node in the parameter or the node to the right of it  
  int nodeHeight(NODE* thisNode){

      if(thisNode == nullptr) //-1 if empty
          return -1;
      else //else return the height
         return thisNode->Height;

  }

  int heightRight(NODE* thisNode){ //get height of right node

      if(thisNode->isThreaded == true) //if its threaded, its a left node, dont return height of threaded right node
          return -1;
      else //else return height of right node
          return thisNode->Right->Height;

  }

  // 
  // search:
  //
  // Searches the tree for the given key, returning true if found
  // and false if not.  If the key is found, the corresponding value
  // is returned via the reference parameter.
  //
  // Time complexity:  O(lgN) on average
  //
  bool search(KeyT key, ValueT& value) const
  {
    NODE* cur = Root;

    while (cur != nullptr)
    {
      if (key == cur->Key)  // already in tree
      {
        value = cur->Value;
        return true;
      }

      if (key < cur->Key) //search left
      {
        cur = _getActualLeft(cur);
      }
      else //else search right
      {
        cur = _getActualRight(cur);
      }
    }

    // if get here, not found
    return false;
  }
  
  //
  // _LeftRotate
  //
  // Rotates the tree around the node N, where Parent is N's parent.  Note that
  // Parent could be null, which means N is the root of the entire tree.  If 
  // Parent denotes a node, note that N could be to the left of Parent, or to
  // the right.  You'll need to take all this into account when linking in the
  // new root after the rotation.  Don't forget to update the heights as well.
  //
  void leftRotate(NODE* Parent, NODE* N)
  {
     NODE* R = N->Right;
     NODE* B = R->Left;
     
     
     R->Left = N;
     N->Right = B;
     
     if(B == nullptr) {
         N->Right = R;
         N->isThreaded = true;
     } else {
         N->isThreaded = false;
     }
     
     if(Parent == nullptr){
        Root = R;
     } else if (R->Key > Parent->Key){
        Parent->Right = R;
     } else if(R->Key < Parent->Key){
        Parent->Left = R;
     }
     N->Height = 1 + max(nodeHeight(N->Left), heightRight(N));
     R->Height = 1 + max(nodeHeight(R->Left), heightRight(R));
  }
  
  //
  // _RightRotate
  //
  // Rotates the tree around the node N, where Parent is N's parent.  Note that
  // Parent could be null, which means N is the root of the entire tree.  If 
  // Parent denotes a node, note that N could be to the left of Parent, or to
  // the right.  You'll need to take all this into account when linking in the
  // new root after the rotation.  Don't forget to update the heights as well.
  //
   void rightRotate(NODE* Parent, NODE* N)
  {
     NODE* L = N->Left;  //label node L
     NODE* B = L->Right; //get the subtree to the right of L
     
     if(L->isThreaded){ //if its threaded, it is already pointing right with its thread, threading is not useful for rotation set to null
        B = nullptr;
     }
     
     N->Left = B; //reset components of new balanced tree
     L->Right = N;
     L->isThreaded = false;
     
     if(Parent == nullptr){ //if we updated at root, reset the root to the proper node L
        Root = L;
     } else if (L->Key < Parent->Key){ //else readjust tree properly
        Parent->Left = L;
     } else{
        Parent->Right = L;
     }
     
     N->Height = 1 + max(nodeHeight(N->Left), heightRight(N)); //update heights of N and L nodes (only ones that changed)
     L->Height = 1 + max(nodeHeight(L->Left), heightRight(L));

  }
  
  //
  // insert
  //
  // Inserts the given key into the tree; if the key has already been insert then
  // the function returns without changing the tree.
  //
  // Time complexity:  O(lgN) on average
  //
  void insert(KeyT key, ValueT value)
  {
    NODE* prev = nullptr; //set a prev node ptr
    NODE* cur = Root; //set cur node ptr
    stack<NODE*> nodes; //initialize stack of nodes

    //
    // 1. Search to see if tree already contains key:
    //
    while (cur != nullptr)
    {
      if (key == cur->Key)  // already in tree
        return;
      
      nodes.push(cur); //push the node to the stack

      if (key < cur->Key)  // search left:
      {
        prev = cur;
        cur = _getActualLeft(cur);
      }
      else
      {
        prev = cur;
        if(cur->isThreaded) 
            cur = nullptr;
        else 
            cur = cur->Right;
      }
    }//while

    //
    // 2. if we get here, key is not in tree, so allocate
    // a new node to insert:
    // 
    NODE* n = new NODE();
    n->Key = key;
    n->Value = value;
    n->Height = 0;
    n->Left = nullptr;
    n->Right = nullptr;
    n->isThreaded = true;
    

    //
    // 3. link in the new node:
    //
    // NOTE: cur is null, and prev denotes node where
    // we fell out of the tree.  if prev is null, then
    // the tree is empty and the Root pointer needs 
    // to be updated.
    //
    if(prev == nullptr)
      {
         Root = n;  
      } else if(key < prev->Key) {
         n->Right = prev; 
         prev->Left = n;
      } else {
         n->Right = prev->Right;          
         prev->isThreaded = false;
         prev->Right = n; 
      }
      Size++;
       
	//while the stack of nodes is not empty
    while(!nodes.empty()){
      cur = nodes.top(); //set cur node to the top node
      nodes.pop(); //pop it

      int HL = -1;
      int HR = -1;
      int HC = 0;
      int BF = 0;

      if(cur->Left == nullptr) { //check left heights for balance factor
         HL = -1;
      } else {
         HL = cur->Left->Height;
      }

      if(cur->isThreaded == false){ //if its not threaded, get the right nodes height for balancing
         HR = cur->Right->Height;
      }

      HC = 1 + max(HL, HR); 

      BF = abs(HL - HR); //get balance factor of current node
      
      if(HC == cur->Height){ //if the tree is balanced we can break
          break;
      }

      cur->Height = HC; //set current height to proper balanced height
      NODE* parent  = nullptr; //set parent to null

      if(!nodes.empty()){
          parent = nodes.top();
      }else{
          parent = nullptr;
      }
      
      if(BF > 1){ //if the balance factor is greater than 1, we have to rotate to rebalance the tree, 4 cases to rebalance
          
      int hLeftLeft = -1;
      int hLeftRight = -1;
      int hRightLeft = -1;
      int hRightRight = -1;
          
         if(HR > HL){ //if right height is greater than left height, we need to rebalance via case 3 or 4
            if(cur->Right->Left != NULL){
                hRightLeft = cur->Right->Left->Height;
            }
            if(cur->Right->isThreaded == false){
                hRightRight = cur->Right->Right->Height;
            }

            // case 4: left rotate
            if(hRightRight > hRightLeft){
                leftRotate(parent, cur);
            }
            else{ //else case 3: right left rotate
                rightRotate(cur, cur->Right);
                leftRotate(parent, cur);
            }
         } else if(HL > HR){ //if left height is greater than right height, its either case 1 or 2 (RR or LR RR)

            if(cur->Left->Left != NULL){
                hLeftLeft = cur->Left->Left->Height;
            }
            if(cur->Left->isThreaded == false){
                hLeftRight = cur->Left->Right->Height;
            }

            // case 1: right rotate
            if(hLeftLeft > hLeftRight){
                rightRotate(parent, cur);
            }
            //else case 2: left right
            else{
                leftRotate(cur, cur->Left);
                rightRotate(parent, cur);
            }

          }
      }
    }
 }

  //
  // []
  //
  // Returns the value for the given key; if the key is not found,
  // the default value ValueT{} is returned.
  //
  // Time complexity:  O(lgN) on average
  //
  ValueT operator[](KeyT key) const
  {
    ValueT  value = ValueT{};
	
    // perform search, and either we find it --- and value is 
    // updated --- or we don't and we'll return default value:
    search(key, value);

    return value;
  }

  //
  // ()
  //
  // Finds the key in the tree, and returns the key to the "right".
  // If the right is threaded, this will be the next inorder key.
  // if the right is not threaded, it will be the key of whatever
  // node is immediately to the right.
  //
  // If no such key exists, or there is no key to the "right", the
  // default key value KeyT{} is returned.
  //
  // Time complexity:  O(lgN) on average
  //
  KeyT operator()(KeyT key) const
  {
    NODE* cur = Root;

    //
    // we have to do a traditional search, and then work from
    // there to follow right / thread:
    //
    while (cur != nullptr)
    {
      if (key == cur->Key)  // found it:
      {
        // 
        // we want the key to the right, either immediately right
        // or the threaded inorder key.  Just need to make sure
        // we have a pointer first:
        if (cur->Right == nullptr)  // threaded but null:
          return KeyT{ };
        else
          return cur->Right->Key;
      }

      if (key < cur->Key)  // search left:
      {
        cur = _getActualLeft(cur);
      }
      else
      {
        cur = _getActualRight(cur);
      }
    }//while  

    //
    // if get here, not found, so return default key:
    //
    return KeyT{ };
  }

  // %
//
// Returns the height stored in the node that contains key; if key is
// not found, -1 is returned.
//
// Example: cout << tree%12345 << endl;
//
// Time complexity: O(lgN) worst - case
//
int operator% (KeyT key) const
{
NODE* cur = Root;

while(cur != nullptr){
  if (key == cur->Key) { //found key we're searching for, return height
    return cur->Height;
  } 
  if (key < cur->Key) //search left:
  {
    cur = _getActualLeft(cur);
  } else {  //right
    cur = _getActualRight(cur);
  }
}
return -1;

}


  //
  // begin
  //
  // Resets internal state for an inorder traversal.  After the 
  // call to begin(), the internal state denotes the first inorder
  // key; this ensure that first call to next() function returns
  // the first inorder key.
  //
  // Space complexity: O(1)
  // Time complexity:  O(lgN) on average
  //
  // Example usage:
  //    tree.begin();
  //    while (tree.next(key))
  //      cout << key << endl;
  //
  void begin()
  {
    Next = Root;

    if (Next != nullptr)  // advance as left as possible:
    {
      while (Next->Left != nullptr)
        Next = Next->Left;
    }
  }

  //
  // next
  //
  // Uses the internal state to return the next inorder key, and 
  // then advances the internal state in anticipation of future
  // calls.  If a key is in fact returned (via the reference 
  // parameter), true is also returned.
  //
  // False is returned when the internal state has reached null,
  // meaning no more keys are available.  This is the end of the
  // inorder traversal.
  //
  // Space complexity: O(1)
  // Time complexity:  O(lgN) on average
  //
  // Example usage:
  //    tree.begin();
  //    while (tree.next(key))
  //      cout << key << endl;
  //
  bool next(KeyT& key)
  {
    if (Next == nullptr)
    {
      // no more keys:

      return false;
    }

    // we have at least one more, so grab it now,
    // advance, and return true:

    key = Next->Key;

    if (_getActualRight(Next) == nullptr)
    {
      // follow the thread:
      Next = Next->Right;
    }
    else
    {
      Next = Next->Right;// go right, and then left as far as we can:
      
      while (Next->Left != nullptr)
        Next = Next->Left;
    }

    return true;
  }

  
  //range search helper function
  bool searchKey(KeyT key) const
  {
    NODE* cur = Root;

    while (cur != nullptr)
    {
      if (key == cur->Key) {
        return true;
      }

      if (key < cur->Key ) {
        cur = _getActualLeft(cur);
      }
      else {
        cur = _getActualRight(cur);
      }
    }//while  

    // if get here, not found
    return false;
  }


  //
  // range_search
  //
  // Searches the tree for all keys in the range [lower..upper], inclusive.
  // It is assumed that lower &lt;= upper.  The keys are returned in a vector;
  // if no keys are found, then the returned vector is empty.
  //
  // Time complexity: O(lgN + M), where M is the # of keys in the range
  // [lower..upper], inclusive.
  //
  // NOTE: do not simply traverse the entire tree and select the keys
  // that fall within the range.  That would be O(N), and thus invalid.
  // Be smarter, you have the technology.
  //
  vector<KeyT> range_search(KeyT lower, KeyT upper)
  {
    vector<KeyT>  keys; //initialize vector
    while(lower <= upper){ //while lower <= upper 
        if(searchKey(lower) == true){ //search for a key at lower, if found
            keys.push_back(lower); //push back the key found within the limit
            lower++; //increment lower
        }else{ //else not found, increment lower
            lower++;
        }
    }
	//if we get here, we searched for every possible key in the tree and pushed it into the vector,
    return keys; //return the vector
  }


  //
  // dump
  // 
  // Dumps the contents of the tree to the output stream, using a
  // recursive inorder traversal.
  //
  void dump(ostream& output) const
  {
	output << "**************************************************" << endl;
    output << "********************* AVLT ***********************" << endl;

    output << "** size: " << this->size() << endl;
    output << "** height: " << this->height() << endl;

    _dump(output, Root);

    output << "**************************************************" << endl;
  }
};