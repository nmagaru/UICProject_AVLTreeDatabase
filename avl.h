#pragma once

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <stack>
#include <vector>
#include <cassert>

using namespace std;

template<typename TKey, typename TValue>
class avltree
{
private:
	struct NODE
	{
		TKey Key;
		TValue Value;
		int Height;
		NODE* Left;
		NODE* Right;
	};
	
	NODE* Root;
	int Size;
	
	// _height private function to get height of tree 
	// private instead of public for rotate functions
	int _height(NODE* curNode)
	{
		if (curNode == nullptr)
			return -1; // curnode does not exist, height is -1 not 0
		else
			return curNode->Height;
	}

	// _inorder private function for inorder traversal
	void _inorder(NODE* curNode)
	{
		if (curNode == nullptr)
			return;
		else
		{
			_inorder(curNode->Left);
			cout << "(" << curNode->Key 
				 << "," << curNode->Value
				 << "," << curNode->Height
				 << ") ";
			_inorder(curNode->Right);
		}
	}
	
	// _inorder_keys private function for key inorder traversal
	void _inorder_keys(NODE* curNode, std::vector<TKey>& vect)
	{
		if (curNode == nullptr)
			return;
		else
		{
			_inorder_keys(curNode->Left, vect);
			vect.push_back(curNode->Key);
			_inorder_keys(curNode->Right, vect);
		}
	}
	
	// _inorder_values private function for value inorder traversal
	void _inorder_values(NODE* curNode, std::vector<TValue>& vect)
	{
		if (curNode == nullptr)
			return;
		else
		{
			_inorder_values(curNode->Left, vect);
			vect.push_back(curNode->Value);
			_inorder_values(curNode->Right, vect);
		}
	}
	
	// _inorder_heights private function for height inorder traversal
	void _inorder_heights(NODE* curNode, std::vector<int>& vect)
	{
		if (curNode == nullptr)
			return;
		else
		{
			_inorder_heights(curNode->Left, vect);
			vect.push_back(curNode->Height);
			_inorder_heights(curNode->Right, vect);
		}
	}
	
	// _cleartree private function to empty tree/free up tree memory
	void _cleartree(NODE* curNode)
	{
		if (curNode == nullptr)
			return;
		else
		{
			_cleartree(curNode->Left);
			_cleartree(curNode->Right);
			delete curNode;
			Size--; // decrease size for every deleted node
		}
	}
	
	// _copytree private function to create a copied tree
	void _copytree(NODE* curNode)
	{
		if (curNode == nullptr)
			return;
		else
		{
			// reinsert each node into tree copy
			insert(curNode->Key, curNode->Value);
			
			_copytree(curNode->Left);
			_copytree(curNode->Right);
		}
	}
	
	// _distancefromnode private function to determine key distance from given node
	int _distancefromnode(NODE* curNode, TKey key)
	{
		if (curNode->Key == key)
			return 0; // key matches given node, 0 distance
		else if (curNode->Key > key)
			return 1 + _distancefromnode(curNode->Left, key);
		else
			return 1 + _distancefromnode(curNode->Right, key);
	}
	
	// _distancebetween private function to determing the shortest distance between keys
	int _distancebetween(NODE* curNode, TKey k1, TKey k2)
	{
		// both keys on left side of tree/subtree
		if (curNode->Key > k1 && curNode->Key > k2)
			return _distancebetween(curNode->Left, k1, k2);
		// both keys on right side of tree/subtree
		else if (curNode->Key < k1 && curNode->Key < k2)
			return _distancebetween(curNode->Right, k1, k2);
		// keys are in opoosite directions (or comparison for reverse pairs)
		else if ( (curNode->Key >= k1 && curNode->Key <= k2) ||
				  (curNode->Key <= k1 && curNode->Key >= k2) )
			return _distancefromnode(curNode, k1) +
				   _distancefromnode(curNode, k2);
	}
	
	// _rightrotate private function to rotate right around given node
	void _rightrotate(NODE* parentNode, NODE* curNode)
	{
		assert(curNode != nullptr);
		assert(curNode->Left != nullptr);
		
		NODE* leftNode = curNode->Left;
		NODE* leafNode = leftNode->Right;
		
		curNode->Left = leafNode;
		leftNode->Right = curNode;
		
		// check parent node on where to link rotated node
		if (parentNode == nullptr)
			Root = leftNode;
		else if (leftNode->Key < parentNode->Key)
			parentNode->Left = leftNode;
		else
			parentNode->Right = leftNode;
		
		// update rotated node heights
		curNode->Height = std::max(_height(curNode->Left),
								   _height(curNode->Right)) + 1;
		leftNode->Height = std::max(_height(leftNode->Left),
								    _height(leftNode->Right)) + 1;
	}
	
	// _leftrotate private function to rotate left around given node
	void _leftrotate(NODE* parentNode, NODE* curNode)
	{
		assert(curNode != nullptr);
		assert(curNode->Right != nullptr);
		
		NODE* rightNode = curNode->Right;
		NODE* leafNode = rightNode->Left;
		
		curNode->Right = leafNode;
		rightNode->Left = curNode;
		
		// check parent node on where to link rotated node
		if (parentNode == nullptr)
			Root = rightNode;
		else if (rightNode->Key < parentNode->Key)
			parentNode->Left = rightNode;
		else
			parentNode->Right = rightNode;
		
		// update rotated node heights
		curNode->Height = std::max(_height(curNode->Left),
								   _height(curNode->Right)) + 1;
		rightNode->Height = std::max(_height(rightNode->Left),
								     _height(rightNode->Right)) + 1;
	}
	
	// _rotatetofix private function to rotate the tree if BST is not AVL
	void _rotatetofix(NODE* parentNode, NODE* curNode)
	{
		assert(curNode != nullptr);
	
		int subLeft, subRight;
		// check if left subtree is higher than right subtree
 		if (_height(curNode->Left) > _height(curNode->Right))
		{
			// compare left subtree's subtrees and rotate accordingly 
			if (curNode->Left->Left != nullptr)
				subLeft = curNode->Left->Left->Height;
			else
				subLeft = -1;
				
			if (curNode->Left->Right != nullptr)
				subRight = curNode->Left->Right->Height;
			else
				subRight = -1;
				
			if (subLeft > subRight)
				_rightrotate(parentNode, curNode);
			else
			{
				_leftrotate(curNode, curNode->Left);
 				_rightrotate(parentNode, curNode);
			}
		}
		// else, right subtree is higher than left subtree
		else
		{
			// compare right subtree's subtrees and rotate accordingly 
			if (curNode->Right->Left != nullptr)
				subLeft = curNode->Right->Left->Height;
			else
				subLeft = -1;
				
			if (curNode->Right->Right != nullptr)
				subRight = curNode->Right->Right->Height;
			else
				subRight = -1;
				
			if (subLeft < subRight)
				_leftrotate(parentNode, curNode);
			else
			{
				_rightrotate(curNode, curNode->Right);
 				_leftrotate(parentNode, curNode);
			}
		}
	}
	

public:
	// default contructor
	avltree()
	{
		Root = nullptr;
		Size = 0;
	}
	
	// copy constructor
	avltree(const avltree& other)
	{
		Root = nullptr;
		Size = 0;
		
		_copytree(other.Root);
	}
	
	// destructor
	virtual ~avltree()
	{
		_cleartree(Root);
	}
	
	// size function (num of nodes in tree)
	int size()
	{
		return Size;
	}
	
	// height function (height of tree, -1 if no nodes)
	int height()
	{
		if (Root == nullptr)
			return -1;
		else
			return _height(Root);
	}
	
	// clear function
	void clear()
	{
		_cleartree(Root);
		
		// reset root and size for new tree
		Root = nullptr;
		Size = 0;
	}
	
	// search function (search tree for given key)
	TValue* search(TKey key)
	{
		NODE* curNode = Root;
		
		// traverse tree
		while (curNode != nullptr)
		{
			// key is found, return node's value
			if (key == curNode->Key)
				return &curNode->Value;
			
			if (key < curNode->Key)
				curNode = curNode->Left;
			else
				curNode = curNode->Right;
		}
		
		// key not found, return null
		return nullptr;
	}
	
	// insert function (insert node into tree appropriately)
	void insert(TKey key, TValue value)
	{
		NODE* prevNode = nullptr;
		NODE* curNode = Root;
		
		stack<NODE*> treeNodes;
		
		// search tree if key is in tree previously
		while (curNode != nullptr)
		{
			if (key == curNode->Key)
				return;
				
			treeNodes.push(curNode);
			
			if (key < curNode->Key)
			{
				prevNode = curNode;
				curNode = curNode->Left;
			}
			else
			{
				prevNode = curNode;
				curNode = curNode->Right;
			}
		}
		
		NODE* newNode = new NODE();
		newNode->Key = key;
		newNode->Value = value;
		newNode->Height = 0;
		newNode->Left = nullptr;
		newNode->Right = nullptr;
		
		if (prevNode == nullptr)
			Root = newNode;
		else if (key < prevNode->Key)
			prevNode->Left = newNode;
		else
			prevNode->Right = newNode;
			
		Size++;
			
		while (!treeNodes.empty())
		{
			curNode = treeNodes.top();
			treeNodes.pop();
			
			int leftHeight, rightHeight, curHeight;
			// take in height of left subtree
			if (curNode->Left == nullptr)
				leftHeight = -1;
			else
				leftHeight = curNode->Left->Height;
			
			// take in height of right subtree
			if (curNode->Right == nullptr)
				rightHeight = -1;
			else
				rightHeight = curNode->Right->Height;
			
			curHeight = 1 + std::max(leftHeight, rightHeight);

			if (curNode->Height == curHeight)
				break;
			else                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
				curNode->Height = curHeight;
				
			// track previous node from current node
			if (treeNodes.empty())
				prevNode = nullptr;
			else
				prevNode = treeNodes.top();
				
			// if AVL status broken with height difference > 1, rotate tree accordingly
			if (std::abs(leftHeight - rightHeight) > 1)
				_rotatetofix(prevNode, curNode);
		}

		return;
	}
	
	// distance function (distance between two given keys)
	int distance(TKey k1, TKey k2)
	{
		int keyDistance;
		NODE* curNode = Root;
		
		if (search(k1) == nullptr || search(k2) == nullptr)
			keyDistance = -1; // either key doesnt exist in tree, distance cannot exist
		else if (k1 == k2)
			keyDistance = 0; // keys are same, negligible distance
		else
			keyDistance = _distancebetween(curNode, k1, k2);
			
		return keyDistance;	
	}
	
	// inorder function (print out keys inorder)
	void inorder()
	{
		cout << "Nodes in order: ";
		
		_inorder(Root);
		
		cout << endl;
	}
	
	// inorder keys function (add node keys inorder)
	std::vector<TKey> inorder_keys()
	{
		std::vector<TKey> keysVect;
		
		_inorder_keys(Root, keysVect);
		
		return keysVect;
	}
	
	// inorder values function (add node values inorder)
	std::vector<TValue> inorder_values()
	{
		std::vector<TValue> valuesVect;
		
		_inorder_values(Root, valuesVect);
		
		return valuesVect;
	}
	
	// inorder heights function (add node heights inorder)
	std::vector<int> inorder_heights()
	{
		std::vector<int> heightsVect;
		
		_inorder_heights(Root, heightsVect);
		
		return heightsVect;
	}
};