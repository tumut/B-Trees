#include "Block.hpp"

#include <algorithm>

// --- //

template<class TKey, std::size_t M>
void BTree<TKey, M>::BNode::initialize(bool isLeaf, int keysCount) {
	this->offset = -1;
	this->isLeaf = isLeaf;
	this->keysCount = keysCount;
}

template<class TKey, std::size_t M>
bool BTree<TKey, M>::BNode::isFull() const {
	return keysCount == 2 * M;
}

template<class TKey, std::size_t M>
std::unique_ptr<TKey> BTree<TKey, M>::BNode::seek(const TKey& key, BTree& tree) const {
	auto it = std::lower_bound(keys, keys + keysCount, key);
	
	if (it == keys + keysCount || key < *it) {
		if (isLeaf) {
			return nullptr;
		}
		else {
			auto node = tree.readFromDisk(children[it - keys]);
			return node.var.seek(key, tree);
		}
	}
	else {
		return std::make_unique<TKey>(*it);
	}
}

// --- //

template<class TKey, std::size_t M>
BTree<TKey, M>::BTree()
	: m_file(nullptr)
	, m_stats({ 0, 0, 0 })
{	}

template<class TKey, std::size_t M>
BTree<TKey, M>::~BTree() {
	if (m_file) std::fclose(m_file);
}

template<class TKey, std::size_t M>
typename BTree<TKey, M>::Statistics BTree<TKey, M>::getStatistics(bool includeFileBlockCount) const {
	auto stats = m_stats;
	stats.blocksInDisk = readHeader().var.blockCount;
	return stats;
}

template<class TKey, std::size_t M>
void BTree<TKey, M>::resetStatistics() {
	m_stats.blocksRead = m_stats.blocksCreated = m_stats.blocksInDisk = 0;
}

template<class TKey, std::size_t M>
void BTree<TKey, M>::finishInsertions() {
	Block<FileHeader> header = readHeader();
	header.var.blockCount = m_stats.blocksCreated;
	writeHeader(header);
}

template<class TKey, std::size_t M>
Block<typename BTree<TKey, M>::BNode> BTree<TKey, M>::readFromDisk(long offset) {
	Block<BNode> node;
	
	std::fseek(m_file, offset, SEEK_SET);
	std::fread(reinterpret_cast<char*>(&node), 1, sizeof(node), m_file);
	
	++m_stats.blocksRead;
	return node;
}

template<class TKey, std::size_t M>
void BTree<TKey, M>::writeToDisk(Block<BNode>& node) {
	if (node.var.offset == -1) {
		std::fseek(m_file, 0, SEEK_END);
		node.var.offset = std::ftell(m_file);
		++m_stats.blocksCreated;
	}
	
	std::fseek(m_file, node.var.offset, SEEK_SET);
	std::fwrite(reinterpret_cast<const char*>(&node), 1, sizeof(node), m_file);
}

template<class TKey, std::size_t M>
Block<typename BTree<TKey, M>::FileHeader> BTree<TKey, M>::readHeader() const {
	Block<FileHeader> header;
	
	std::fseek(m_file, 0, SEEK_SET);
	std::fread(reinterpret_cast<char*>(&header), 1, sizeof(header), m_file);
	
	return header;
}

template<class TKey, std::size_t M>
void BTree<TKey, M>::writeHeader(const Block<FileHeader>& header) {
	std::fseek(m_file, 0, SEEK_SET);
	std::fwrite(reinterpret_cast<const char*>(&header), 1, sizeof(FileHeader), m_file);
}

template<class TKey, std::size_t M>
bool BTree<TKey, M>::create(const char* filepath) {
	if (m_file) std::fclose(m_file);
	
	if (m_file = std::fopen(filepath, "wb+")) {
		resetStatistics();
		
		Block<FileHeader> header;
		writeHeader(header);
		++m_stats.blocksCreated;
		
		m_root.var.initialize(true); // Root begins as a leaf
		writeToDisk(m_root);
		
		header.var.rootAddress = m_root.var.offset;
		header.var.blockCount = m_stats.blocksCreated;
		writeHeader(header);
		
		return true;
	}
	else {
		return false;
	}
}

template<class TKey, std::size_t M>
bool BTree<TKey, M>::load(const char* filepath) {
	if (m_file) std::fclose(m_file);
	
	if (m_file = std::fopen(filepath, "rb")) {
		Block<FileHeader> header = readHeader();
		m_root = readFromDisk(header.var.rootAddress);
		++m_stats.blocksRead;
		return true;
	}
	else {
		return false;
	}
}

template<class TKey, std::size_t M>
void BTree<TKey, M>::insert(const TKey& key) {
	if (auto overflow = insert(m_root, key)) {
		Block<BNode> newRoot;
		newRoot.var.initialize(false, 1);
		newRoot.var.keys[0] = overflow->middle;
		newRoot.var.children[0] = m_root.var.offset;
		newRoot.var.children[1] = overflow->rightNode;
		writeToDisk(newRoot);
		
		auto header = readHeader();
		header.var.rootAddress = newRoot.var.offset;
		header.var.blockCount = m_stats.blocksCreated;
		writeHeader(header);
		
		m_root = newRoot;
	}
}

template <class TKey, std::size_t M>
std::unique_ptr<TKey> BTree<TKey, M>::seek(const TKey& key) {
	return m_root.var.seek(key, *this);
}

template<class TKey, std::size_t M>
std::unique_ptr<typename BTree<TKey, M>::OverflowResult> BTree<TKey, M>::insert(Block<BNode>& node, TKey key, long rightNodeOffset) {
	auto end = node.var.keys + node.var.keysCount;
	auto iter = std::lower_bound(node.var.keys, end, key);
	
	auto i = iter - node.var.keys;
	
	if (!node.var.isLeaf && rightNodeOffset == -1) {
		Block<BNode> next = readFromDisk(node.var.children[i]);
		
		if (auto overflow = insert(next, key)) {
			return insert(node, overflow->middle, overflow->rightNode);
		}
	}
	else {
		for (auto j = node.var.keysCount; i < j; --j) {
			node.var.keys[j] = node.var.keys[j - 1];
		}
		
		node.var.keys[i] = key;
		
		if (!node.var.isLeaf) {
			for (auto j = 2 * M + 1; i + 1 < j; --j) {
				node.var.children[j] = node.var.children[j - 1];
			}
			
			node.var.children[i + 1] = rightNodeOffset;
		}
		
		if (node.var.isFull()) {
			Block<BNode> right;
			right.var.initialize(node.var.isLeaf, M);
			
			for (auto j = M + 1; j <= node.var.keysCount; ++j) {
				right.var.keys[j - M - 1] = node.var.keys[j];
			}
			
			if (!node.var.isLeaf) {
				for (auto j = M + 1; j <= node.var.keysCount + 1; ++j) {
					right.var.children[j - M - 1] = node.var.children[j];
				}
			}
			
			node.var.keysCount = M;
			
			writeToDisk(right);
			writeToDisk(node);
			
			auto overflow = std::make_unique<OverflowResult>();
			overflow->middle = node.var.keys[M];
			overflow->rightNode = right.var.offset;
			return overflow;
		}
		else {
			++node.var.keysCount;
			writeToDisk(node);
		}
	}
	
	return nullptr;
}
