#ifndef _BTREE_HPP_INCLUDED_
#define _BTREE_HPP_INCLUDED_

#include "Block.hpp"

#include <cstdio>
#include <memory>
#include <iostream>

template<class TKey, std::size_t M>
class BTree {
public:
	BTree();
	~BTree();
	
	bool create(const char* filepath);
	bool load(const char* filepath);
	void insert(const TKey& key);
	std::unique_ptr<TKey> seek(const TKey& key);
	
	struct Statistics {
		unsigned int blocksRead;
		unsigned int blocksCreated;
		unsigned int blocksInDisk;
	};
	
	Statistics getStatistics(bool includeFileBlockCount = false) const;
	void resetStatistics();
	
	// Updates the header with its total block count
	void finishInsertions();
	
private:
	struct FileHeader {
		long rootAddress;
		unsigned int blockCount;
	};
	
	struct BNode {
		long offset; // Address on disk
		bool isLeaf;
		std::size_t keysCount;
		
		TKey keys[2 * M + 1]; // +1 extra space for overflow
		long children[2 * M + 2]; // +1 extra space for overflow
		
		// Sets an invalid value for offset;
		// Is a method instead of a constructor for BNode to be usable in 
		// the Block union (union requires only POD members).
		void initialize(bool isLeaf = true, int keysCount = 0);
		
		bool isFull() const;
		
		std::unique_ptr<TKey> seek(const TKey& key, BTree& tree) const;
	};
	
	std::FILE *m_file;
	Block<BNode> m_root;
	Statistics m_stats;
	
	Block<BNode> readFromDisk(long offset);
	void writeToDisk(Block<BNode>& node); // Updates the node's offset if not yet defined
	
	Block<FileHeader> readHeader() const;
	void writeHeader(const Block<FileHeader>& header);
	
	struct OverflowResult {
		TKey middle;
		long rightNode;
	};
	
	std::unique_ptr<OverflowResult> insert(Block<BNode>& node, TKey key, long rightNodeOffset = -1);
};

#include "BTree.inl"

#endif // _BTREE_HPP_INCLUDED_
