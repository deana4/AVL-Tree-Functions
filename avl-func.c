#include "avl.h"
#include <stdio.h>
#include <stdlib.h>

int _max( int x, int y ){
	return (x < y) ? y : x;
}

int _abs( int x ){
	return (x < 0) ? -x : x;
}

AVLNodePtr Left_Left_Rotate(AVLNodePtr node);
AVLNodePtr Right_Right_Rotate(AVLNodePtr node);
AVLNodePtr Left_Right_Rotate(AVLNodePtr node);
AVLNodePtr Right_Left_Rotate(AVLNodePtr node);
AVLNodePtr fixer(AVLNodePtr root);
AVLNodePtr avl_join( AVLNodePtr root1, AVLNodePtr xnode, AVLNodePtr root2 );
AVLNodePtr p_successor(AVLNodePtr root);
int height_of_avl(AVLNodePtr root);
int BF_of_avl(AVLNodePtr root);
void swap_nodes(AVLNodePtr root, AVLNodePtr succ_x);


// Operations implementation

AVLNodePtr avl_search( AVLNodePtr root, int x ){
	if (root == NULL){
		return NULL;
	}
	if(root->key == x){
		return root;
	}
	else if(x > root->key){
		return avl_search(root->child[RIGHT],x);
	}
	else {
		return avl_search(root->child[LEFT],x);
	}
}

AVLNodePtr avl_insert( AVLNodePtr root, int x ){
    if (root == NULL){
        return new_avl_node(x);
    }
    if(root->key == x){
        return root;
    }
    else if(root->key > x){
        root->child[LEFT] = avl_insert(root->child[LEFT],x);
    }
    else {
        root->child[RIGHT] = avl_insert(root->child[RIGHT],x);
    }
    root->height = 1 + _max(height_of_avl(root->child[LEFT]),height_of_avl(root->child[RIGHT]));
    if(_abs(BF_of_avl(root)) <= 1){
        return root;
    }
    else {
        if(BF_of_avl(root) > 0 && BF_of_avl(root->child[LEFT]) > 0){
            return Right_Right_Rotate(root);
        }
        else if(BF_of_avl(root) < 0 && BF_of_avl(root->child[RIGHT]) < 0){
            return Left_Left_Rotate(root);
        }
        else if(BF_of_avl(root) > 0 && BF_of_avl(root->child[LEFT]) < 0){
            return Left_Right_Rotate(root);
        }
        else {
            return Right_Left_Rotate(root);
        }
    }
}

AVLNodePtr avl_delete( AVLNodePtr root, int x ){
	if(root == NULL){
		return NULL;
	}
	if(avl_search(root,x) == NULL){
		return root;
	}
	if(root->key == x){
		if(root->child[LEFT] == NULL && root->child[RIGHT] == NULL){
			free(root);
			return NULL;
		}
		else if(root->child[LEFT] == NULL && root->child[RIGHT] != NULL){
			AVLNodePtr temp_x = root->child[RIGHT];
			free(root);
			return temp_x;
		}
		else if(root->child[RIGHT] == NULL && root->child[LEFT] != NULL){
			AVLNodePtr temp_x = root->child[LEFT];
			free(root);
			return temp_x;
		}
		else {
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
		}
	}
	else if(x > root->key){
		root->child[RIGHT] = avl_delete(root->child[RIGHT],x);
	}
	else {
		root->child[LEFT] = avl_delete(root->child[LEFT],x);
	}

	root->height = 1 + _max(height_of_avl(root->child[LEFT]),height_of_avl(root->child[RIGHT]));

	if(_abs(BF_of_avl(root)) <= 1){
		return root;
	}
	else {
		if(BF_of_avl(root) > 0 && BF_of_avl(root->child[LEFT]) >= 0){
			return Right_Right_Rotate(root);
		}
		else if(BF_of_avl(root) < 0 && BF_of_avl(root->child[RIGHT]) <= 0){
			return Left_Left_Rotate(root);
		}
		else if(BF_of_avl(root) > 0 && BF_of_avl(root->child[LEFT]) <= 0){
			return Left_Right_Rotate(root);
		}
		else {
			return Right_Left_Rotate(root);
		}
	}
	return root;
}

AVLNodePtr avl_join( AVLNodePtr root1, AVLNodePtr xnode, AVLNodePtr root2 ){
	AVLNodePtr main_root;
	if(_abs(height_of_avl(root1)-height_of_avl(root2)) <= 1){
		xnode->child[LEFT] = root1;
		xnode->child[RIGHT] = root2;
		xnode->height = _max(height_of_avl(root1), height_of_avl(root2)) + 1;
		return xnode;
	}
	else{
		if(height_of_avl(root1) > height_of_avl(root2)){
			root1->child[RIGHT] = avl_join(root1->child[RIGHT],xnode,root2);
			main_root = root1;
		}
		else{
			root2->child[LEFT] = avl_join(root1,xnode,root2->child[LEFT]);
			main_root = root2;
		}
	}
	return fixer(main_root);
}

AVLNodePtr avl_split( AVLNodePtr root, int x, AVLNodePtr trees_out[2] ){
	AVLNodePtr holder;
	if (root->key == x) {
		trees_out[0] = root->child[LEFT];
		trees_out[1] = root->child[RIGHT];
		return new_avl_node(x);
	}
	else if (x > root->key) {
		holder = avl_split(root->child[RIGHT], x, trees_out);
		trees_out[0] = avl_join(root->child[LEFT],root,trees_out[0]);
		return holder;
	}
	else {
		holder = avl_split(root->child[LEFT], x, trees_out);
		trees_out[1] = avl_join(trees_out[1], root,root->child[RIGHT]);
		return holder;
	}
}

AVLNodePtr new_avl_node( int x ){
	AVLNodePtr NewNode; //make new node
	NewNode = (AVLNodePtr)malloc(sizeof(AVLNode));
	if(NewNode == NULL){
		exit(1);
	}
	NewNode->child[LEFT] = NULL; NewNode->child[RIGHT] = NULL;
	NewNode->height = 0;
	NewNode->key = x;
	return NewNode;
}

void delete_avl_tree( AVLNodePtr root ){
	if(root == NULL){
		return;
	}

	delete_avl_tree(root->child[LEFT]); // go down left to minimal val;
	delete_avl_tree(root->child[RIGHT]); // go down left to minimal val;

	free(root); //if we got into leaf we know we can delete it.
}

//L Rotate;
AVLNodePtr Left_Left_Rotate(AVLNodePtr node){
    AVLNodePtr UpdatedNodePtr = node->child[RIGHT];

    AVLNodePtr Sub_tree_of_Right_child = node->child[RIGHT]->child[LEFT];
    node->child[RIGHT]->child[LEFT] = node;
    node->child[RIGHT] = Sub_tree_of_Right_child;

    node->height =1+ _max(height_of_avl(node->child[LEFT]),height_of_avl(node->child[RIGHT])); //height of node is max height between his childs.
    UpdatedNodePtr->height = 1+ _max(height_of_avl(UpdatedNodePtr->child[LEFT]), height_of_avl(UpdatedNodePtr->child[RIGHT])); //height of new root of subtree is max height between his childs.
    return UpdatedNodePtr;
}

//R Rotate;
AVLNodePtr Right_Right_Rotate(AVLNodePtr node){
    AVLNodePtr UpdatedNodePtr = node->child[LEFT];

    AVLNodePtr Sub_tree_of_Left_child = node->child[LEFT]->child[RIGHT];
    node->child[LEFT]->child[RIGHT] = node;
    node->child[LEFT] = Sub_tree_of_Left_child;

    node->height = 1+ _max(height_of_avl(node->child[LEFT]),height_of_avl(node->child[RIGHT])); //height of node is max height between his childs.
    UpdatedNodePtr->height = 1+ _max(height_of_avl(UpdatedNodePtr->child[LEFT]), height_of_avl(UpdatedNodePtr->child[RIGHT])); //height of new root of subtree is max height between his childs.
    return UpdatedNodePtr;
}

//Left Then Right Rotate;
AVLNodePtr Left_Right_Rotate(AVLNodePtr node){
    node->child[LEFT] = Left_Left_Rotate(node->child[LEFT]); //L Rotate;
    return Right_Right_Rotate(node);
}

//Right Then Left Rotate;
AVLNodePtr Right_Left_Rotate(AVLNodePtr node){
    node->child[RIGHT] = Right_Right_Rotate(node->child[RIGHT]); //R Rotate;
    return Left_Left_Rotate(node);
}

//this function will return the balance factor of node(x)
int BF_of_avl(AVLNodePtr root){
    int l_size, r_size;
    if(root == NULL)
        return 0;
    l_size = height_of_avl(root->child[LEFT]);
    r_size = height_of_avl(root->child[RIGHT]);
    return l_size - r_size;
}

//this function will return the height of some node(x)
int height_of_avl(AVLNodePtr root){
    if(root == NULL)
        return -1;
    return root->height;
}

AVLNodePtr p_successor(AVLNodePtr root){
	if(root->child[LEFT]->child[LEFT] == NULL){
        return root;
	}
	else {
        return p_successor(root->child[LEFT]);
	}
}

void swap_nodes(AVLNodePtr root, AVLNodePtr succ_x){
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

AVLNodePtr fixer(AVLNodePtr root) {
	root->height = 1 + _max(height_of_avl(root->child[LEFT]), height_of_avl(root->child[RIGHT])); //height of node is max height between his childs.
	if (_abs(BF_of_avl(root)) <= 1) {
		return root;
	}
	else {
		if (BF_of_avl(root) > 0 && BF_of_avl(root->child[LEFT]) > 0) {
			return Right_Right_Rotate(root);
		}
		else if (BF_of_avl(root) < 0 && BF_of_avl(root->child[RIGHT]) < 0) {
			return Left_Left_Rotate(root);
		}
		else if (BF_of_avl(root) > 0 && BF_of_avl(root->child[LEFT]) < 0) {
			return Left_Right_Rotate(root);
		}
		else {
			return Right_Left_Rotate(root);
		}
	}
}
