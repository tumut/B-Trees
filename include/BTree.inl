#include "Block.hpp"

#include <algorithm>

// --- //

template<typename T, std::size_t M>
void BTree<T, M>::BNode::initialize(bool isLeaf, int size) {
	this->offset = -1;
	this->isLeaf = isLeaf;
	this->size = size;
}

template<typename T, std::size_t M>
bool BTree<T, M>::BNode::isFull() const {
	return size >= 2 * M;
}

template<typename T, std::size_t M>
std::unique_ptr<T> BTree<T, M>::BNode::seek(const T& key, BTree& tree) const {
	auto it = std::lower_bound(values, values + size, key);
	
	if (it == values + size || key < *it) {
		if (isLeaf) {
			return nullptr;
		}
		else {
			auto node = tree.readFromDisk(children[it - values]);
			return node.var.seek(key, tree);
		}
	}
	else {
		return std::make_unique<T>(*it);
	}
}

// --- //

template<typename T, std::size_t M>
BTree<T, M>::BTree()
	: m_file(nullptr)
	, m_stats({ 0, 0, 0 })
{	}

template<typename T, std::size_t M>
BTree<T, M>::~BTree() {
	if (m_file) std::fclose(m_file);
}

template<typename T, std::size_t M>
bool BTree<T, M>::create(const char* filepath) {
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

template<typename T, std::size_t M>
bool BTree<T, M>::load(const char* filepath) {
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

template <typename T, std::size_t M>
std::unique_ptr<T> BTree<T, M>::seek(const T& key) {
	return m_root.var.seek(key, *this);
}

template<typename T, std::size_t M>
const typename BTree<T, M>::Statistics& BTree<T, M>::getStatistics(bool includeFileBlockCount) const {
	if (includeFileBlockCount)
		m_stats.blocksInDisk = readHeader().var.blockCount;

	return m_stats;
}

template<typename T, std::size_t M>
void BTree<T, M>::resetStatistics() {
	m_stats.blocksRead = m_stats.blocksCreated = m_stats.blocksInDisk = 0;
}

template<typename T, std::size_t M>
void BTree<T, M>::finishInsertions() {
	Block<FileHeader> header = readHeader();
	header.var.blockCount = m_stats.blocksCreated;
	writeHeader(header);
}

template<typename T, std::size_t M>
Block<typename BTree<T, M>::BNode> BTree<T, M>::readFromDisk(long offset) {
	Block<BNode> node;
	
	std::fseek(m_file, offset, SEEK_SET);
	std::fread(reinterpret_cast<char*>(&node), 1, sizeof(node), m_file);
	
	++m_stats.blocksRead;
	return node;
}

template<typename T, std::size_t M>
void BTree<T, M>::writeToDisk(Block<BNode>& node) {
	if (node.var.offset == -1) {
		std::fseek(m_file, 0, SEEK_END);
		node.var.offset = std::ftell(m_file);
		++m_stats.blocksCreated;
	}
	
	std::fseek(m_file, node.var.offset, SEEK_SET);
	std::fwrite(reinterpret_cast<const char*>(&node), 1, sizeof(node), m_file);
}

template<typename T, std::size_t M>
Block<typename BTree<T, M>::FileHeader> BTree<T, M>::readHeader() const {
	Block<FileHeader> header;
	
	std::fseek(m_file, 0, SEEK_SET);
	std::fread(reinterpret_cast<char*>(&header), 1, sizeof(header), m_file);
	
	++m_stats.blocksRead;
	return header;
}

template<typename T, std::size_t M>
void BTree<T, M>::writeHeader(const Block<FileHeader>& header) {
	std::fseek(m_file, 0, SEEK_SET);
	std::fwrite(reinterpret_cast<const char*>(&header), 1, sizeof(FileHeader), m_file);
}

template<typename T, std::size_t M>
void BTree<T, M>::insert(const T& value) {
	if (auto overflow = insert(m_root, value)) {
		Block<BNode> newRoot;
		newRoot.var.initialize(false, 1);
		newRoot.var.values[0] = overflow->middle;
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

template<typename T, std::size_t M>
std::unique_ptr<typename BTree<T, M>::OverflowResult> BTree<T, M>::insert(Block<BNode>& node, T key, long rightNodeOffset) {
	auto end = node.var.values + node.var.size;
	auto iter = std::lower_bound(node.var.values, end, key);
	
	auto i = iter - node.var.values;
	
	if (!node.var.isLeaf && rightNodeOffset == -1) {
		Block<BNode> next = readFromDisk(node.var.children[i]);
		
		if (auto overflow = insert(next, key)) {
			return insert(node, overflow->middle, overflow->rightNode);
		}
	}
	else {
		for (auto j = node.var.size; i < j; --j) {
			node.var.values[j] = node.var.values[j - 1];
		}
		
		node.var.values[i] = key;
		
		if (!node.var.isLeaf) {
			for (auto j = 2 * M + 1; i + 1 < j; --j) {
				node.var.children[j] = node.var.children[j - 1];
			}
			
			node.var.children[i + 1] = rightNodeOffset;
		}
		
		if (node.var.isFull()) {
			Block<BNode> right;
			right.var.initialize(node.var.isLeaf, M);
			
			for (auto j = M + 1; j <= node.var.size; ++j) {
				right.var.values[j - M - 1] = node.var.values[j];
			}
			
			if (!node.var.isLeaf) {
				for (auto j = M + 1; j <= node.var.size + 1; ++j) {
					right.var.children[j - M - 1] = node.var.children[j];
				}
			}
			
			node.var.size = M;
			
			writeToDisk(right);
			writeToDisk(node);
			
			auto overflow = std::make_unique<OverflowResult>();
			overflow->middle = node.var.values[M];
			overflow->rightNode = right.var.offset;
			return overflow;
		}
		else {
			++node.var.size;
			writeToDisk(node);
		}
	}
	
	return nullptr;
}
