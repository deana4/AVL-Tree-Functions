#include "avl.h"
#include <stdio.h>
#include <stdlib.h>
#define COUNT 10

AVLNodePtr BalanceTree(AVLNodePtr root, int balance_factor);
AVLNodePtr RotateRight(AVLNodePtr root);
AVLNodePtr RotateLeft(AVLNodePtr root);
int BF(AVLNodePtr root);
int GetHeight(AVLNodePtr node);
void AssignHeight(AVLNodePtr root);
AVLNodePtr Equalizer(AVLNodePtr root);
AVLNodePtr p_successor(AVLNodePtr root);
void swap_nodes(AVLNodePtr root, AVLNodePtr succ_x);


AVLNodePtr new_avl_node( int x )
{
    AVLNodePtr pnew;
    if(!(pnew = (AVLNodePtr) malloc (sizeof(AVLNode))))
    {
        printf("memory allocation failed./n");
        exit(1);
    }
    pnew->child[0] = pnew->child[1] = NULL;
    pnew->key = x;
    pnew->height = 0;
    return pnew;
}

void delete_avl_tree( AVLNodePtr root ) //delete by post order traversal
{
    if(root == NULL)
        return;
    delete_avl_tree(root->child[0]);
    delete_avl_tree(root->child[1]);
    free(root);
}

AVLNodePtr avl_search( AVLNodePtr root, int x )
{
    while(root)
    {
        if(root->key == x)
        return root;
        if(root->key < x)
            root = root->child[RIGHT];
        else
            root = root->child[LEFT];
    }
    return NULL;
}

/*to save time on the way up we defined a global variable which will be set to 1 if while we "fold" the recursion
  calls we realize that we do not have to check any more. that happens if:
  1) we reached a node whose height hasn't changed as a consequence of the insertion.
  2) we balanced a subtree using rotations.
  3) the value x is in the tree already
  in every insertion, the global variable is set to zero when we begin returning the recursion. */

int insert_balanced;
AVLNodePtr avl_insert( AVLNodePtr root, int x )
{
    if(root == NULL)
    {
        AVLNodePtr pnew = new_avl_node(x);
        insert_balanced = 0;
        return pnew;
    }
    else if(root->key == x)
    {
        insert_balanced = 1;
        return root;
    }
    else if(root->key > x)
    {
        root->child[LEFT] = avl_insert(root->child[LEFT], x);
    }
    else
    {
        root->child[RIGHT] = avl_insert(root->child[RIGHT], x);
    }
    if(insert_balanced)               //this is where we save time on excessive operation.
        return root;
    else
    {
        int balance_factor;
        int h = root->height;
        AssignHeight(root);
        if (h == root->height)       //if (the height has not changed) don't worry, the rest of the tree is balanced.
        {
            insert_balanced = 1;
            return root;
        }
        balance_factor = BF(root);                     //get the balance factor.
        if(_abs(balance_factor) <= 1)                  //if it is 1/-1
            return root;                               //do nothing
        else
        {
            root = BalanceTree(root, balance_factor);  //if it is 2/-2, call the function which balances sub trees
            insert_balanced = 1;                       //don't worry any more
            return root;
        }
    }
}

AVLNodePtr BalanceTree(AVLNodePtr root, int balance_factor)  //does rotations as needed, according to what we studied
{
    if(balance_factor == 2)
    {
        if(BF(root->child[LEFT]) >= 0)
            return RotateRight(root);
        else
        {
            root->child[LEFT] = RotateLeft(root->child[LEFT]);
            root = RotateRight(root);
            return root;
        }
    }
    else
    {
        if(BF(root->child[RIGHT]) <= 0)
            return RotateLeft(root);
        else
        {
            root->child[RIGHT] = RotateRight(root->child[RIGHT]);
            root = RotateLeft(root);
            return root;
        }
    }
}

AVLNodePtr RotateRight(AVLNodePtr root)
{
    AVLNodePtr temp = root->child[LEFT];
    root->child[LEFT] = temp->child[RIGHT];
    temp->child[RIGHT] = root;

    AssignHeight(temp->child[LEFT]);
    AssignHeight(temp->child[RIGHT]);
    AssignHeight(temp);
    return temp;
}

AVLNodePtr RotateLeft(AVLNodePtr root)
{
    AVLNodePtr temp = root->child[RIGHT];
    root->child[RIGHT] = temp->child[LEFT];
    temp->child[LEFT] = root;

    AssignHeight(temp->child[LEFT]);
    AssignHeight(temp->child[RIGHT]);
    AssignHeight(temp);
    return temp;
}

int BF(AVLNodePtr root)      //returns height(left subtree) - height(right subtree)
{
    return (root->child[LEFT] ? root->child[LEFT]->height + 1:0) - (root->child[RIGHT] ? root->child[RIGHT]->height + 1 : 0);
}

int GetHeight(AVLNodePtr node)
{
    if(node == NULL)
        return 0;
    else
        return node->height;
}

void AssignHeight(AVLNodePtr root)
{
    if(!root)
        return;

    if(!root->child[LEFT] && !root->child[RIGHT])
        root->height = 0;
    else
        root->height = _max(GetHeight(root->child[LEFT]), GetHeight(root->child[RIGHT])) + 1;
}

AVLNodePtr avl_delete( AVLNodePtr root, int x )
{
    if(root == NULL)
        return NULL;
    else if(root->key < x)
        root->child[RIGHT] = avl_delete(root->child[RIGHT], x);
    else if(root->key > x)
        root->child[LEFT] = avl_delete(root->child[LEFT], x);
    else                                                     //root->key == x
    {
        if(!root->child[LEFT] && !root->child[RIGHT])        //the simplest case: x is in a node
        {
            free(root);                                      //set the leave loose
            return NULL;
        }
        else if(root->child[LEFT] && root->child[RIGHT])     //if it has 2 children:
        {
            AVLNodePtr succ_x;

            if(root->child[RIGHT]->child[LEFT] == NULL){
                succ_x = root->child[RIGHT];
                swap_nodes(root,succ_x); // swap x with succ(x)
                succ_x->child[RIGHT] = avl_delete(succ_x->child[RIGHT],x); //make sure that x deleted from the tree
            }
            else{
                AVLNodePtr p_succ_of_x = p_successor(root->child[RIGHT]); //get successor parent(x)
                succ_x = p_succ_of_x->child[LEFT]; //get successor
                swap_nodes(root,succ_x); //swap root with successor
                p_succ_of_x->child[LEFT] = root;
                succ_x->child[RIGHT] = avl_delete(succ_x->child[RIGHT],x);
            }
            root = succ_x; //the new root is the successor of x.
            root = Equalizer(root);
            return root;
        }
        else if(root->child[LEFT])                         //there is only left son
        {
            AVLNodePtr LeftSon = root->child[LEFT];
            free(root);
            return LeftSon;
        }
        else                                                //there is only right son
        {
            AVLNodePtr RightSon = root->child[RIGHT];
            free(root);
            return RightSon;
        }
    }
    root = Equalizer(root);                                 //while deleting, as opposed to inserting
    return root;                                            //we should check the balance all the way up.
}                                                           //and Equalizer does it all.


AVLNodePtr Equalizer(AVLNodePtr root)                   //Equalizer, as its name suggests, equalize the sub tree
{                                                       //by assigning height, checking the BF and rotating, if needed.

    int balance_factor;
    AssignHeight(root);
    balance_factor = BF(root);
    if(_abs(balance_factor) == 2)
        root = BalanceTree(root, balance_factor);
    return root;
}

int _max( int x, int y )
{
    return x>y?x:y;
}

int _abs( int x )
{
    return x>0?x:-x;
}

AVLNodePtr avl_join( AVLNodePtr root1, AVLNodePtr xnode, AVLNodePtr root2 )
{
    int root1_height = GetHeight(root1);               //get the heights
    int root2_height = GetHeight(root2);
    if(_abs(root1_height - root2_height) <= 1)         //if (Delta <= 1) connect them
    {
        xnode->child[RIGHT] = root2;
        xnode->child[LEFT] = root1;
        xnode = Equalizer(xnode);
        return xnode;
    }
    else if(root1_height > root2_height)                //else, continue descending as needed
        root1->child[RIGHT] = avl_join(root1->child[RIGHT], xnode, root2);
    else
        root2->child[LEFT] = avl_join(root1, xnode, root2->child[LEFT]);

    if(root1_height > root2_height)     //equalize and return the correct tree
    {
        root1 = Equalizer(root1);
        return root1;
    }
    else
    {
        root2 = Equalizer(root2);
        return root2;
    }
}

AVLNodePtr avl_split( AVLNodePtr root, int x, AVLNodePtr trees_out[2] )
{
    AVLNodePtr split_node;
    if(root->key == x)                                    //split 'em right  here
    {
        trees_out[0] = root->child[LEFT];
        trees_out[1] = root->child[RIGHT];
        root->child[LEFT] = root->child[RIGHT] = NULL;
        return root;
    }
    else if(root->key > x)
        split_node =  avl_split(root->child[LEFT], x, trees_out);
    else
        split_node = avl_split(root->child[RIGHT], x, trees_out);

    if(root->key < x)                                      //the join function does the equalizing
        trees_out[0] = avl_join(root->child[LEFT], root, trees_out[0]);
    else
        trees_out[1] = avl_join(trees_out[1], root, root->child[RIGHT]);
    return split_node;
}

AVLNodePtr p_successor(AVLNodePtr root){           //returns the successor father
	if(root->child[LEFT]->child[LEFT] == NULL){
        return root;
	}
	else {
        return p_successor(root->child[LEFT]);
	}
}

void swap_nodes(AVLNodePtr root, AVLNodePtr succ_x){  //swaps tree nodes
    AVLNodePtr temp_s_r = succ_x->child[RIGHT];
    AVLNodePtr temp_s_l = succ_x->child[LEFT];

	succ_x->child[RIGHT] = root->child[RIGHT];
	succ_x->child[LEFT] = root->child[LEFT];
	if(succ_x->child[RIGHT] == succ_x){
        succ_x->child[RIGHT] = root;
	}
	root->child[LEFT] = temp_s_l;
	root->child[RIGHT] = temp_s_r;

	int tmp = root->height;
	root->height = succ_x->height;
	succ_x->height = tmp;
}
