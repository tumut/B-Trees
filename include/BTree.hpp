#ifndef _BTREE_HPP_INCLUDED_
#define _BTREE_HPP_INCLUDED_

#include <cstdio>
#include <memory>

#include "Block.hpp"

//! B-tree class
/*!
 * BTree can be used to store T-type values in binary files for fast retrieval
 * later. T must be a POD (Plain Old Data type) and _less-than_ comparable.
 *
 * Use the method BTree::create before inserting values and, once you've
 * finished inserting, call BTree::finishInsertions to update the file header.
 *
 * Use BTree::load before reading values.
 *
 * Example usage:
 * \code
 * BTree<int, 2> tree;
 *
 * // Insert data
 * tree.create("filename.bin");
 * tree.insert(1);
 * tree.finishInsertions();
 *
 * // Read data
 * tree.load("filename.bin");
 * int *x = tree.seek(1);
 * if (x) f(*x); // If found, do something to x
 * \endcode
 *
 * @tparam T Type of the data to be stored
 *
 * @tparam M Tree order. Each node will store up to 2M values and have up to
 * 2M + 1 children. A node can't be bigger than `BLOCK_SIZE` bytes.
 *
 * @tparam BlockSize Block size to use, in bytes
 */
template<typename T, std::size_t M, unsigned int BlockSize = BLOCK_SIZE>
class BTree {
public:
	//! Type of the stored values
	typedef T ValueType;

	//! Tree order
	static constexpr auto Order = M;

	//! Block size in bytes
	static constexpr auto BlockSizeInUse = BlockSize;

	//! Default constructor
	/*!
	 * Initializes the statistics with 0 values and makes the file pointer
	 * point to null
	 */
	BTree();
	
	//! Destructor
	/*! Closes the file if it's open */
	~BTree();
	
	//! Initializes BTree for writing
	/*!
	 * Opens the file in "wb+" mode. Must be called before inserting values in
	 * the tree. Also allows reading through BTree::seek.
	 *
	 * If there's already a file in the filepath, it'll be overwritten.
	 *
	 * The new file will be created in the provided filepath with a header
	 * and an empty root node.
	 *
	 * After you've finished inserting values, call BTree::finishInsertions to
	 * update the file header.
	 *
	 * @param filepath Path to the file where the tree data will be written
	 *
	 * @return True if the file was created successfully
	*/
	bool create(const char* filepath);
	
	//! Initializes BTree for reading only
	/*!
	 * Opens the file in "rb" mode. Must be before reading values form a file
	 * where values have already been inserted. Insertions won't be possible.
	 *
	 * The file must have been previously created by a BTree which successfully
	 * called BTree::create and BTree::finishInsertions.
	 *
	 * @param filepath Path to the file where tree data can be found
	 *
	 * @return True if it was possible to open the file in filepath
	 */
	bool load(const char* filepath);
	
	//! Inserts a value in the tree
	/*!
	 * @param value Value to insert
	 */
	void insert(const T& value);
	
	//! Seeks a value that's equivalent to the one provided
	/*!
	 * If the value is not found, a null pointer will be returned.
	 *
	 * You can optionally provide a custom comparer. Useful in case you want to
	 * store key-value pairs within the B-tree; in such a case, you'll want to
	 * compare the provided parameter key with the pairs' keys.
	 *
	 * @tparam U Type of the key to seek. T and U must be less-than comparable.
	 *
	 * @param key The value to seek
	 * @param less Comparer instance
	 *
	 * @return Pointer with the value if found, null otherwise
	 */
	template <typename U>
	std::unique_ptr<T> seek(const U& key);
	
	//! Stored analytics of the BTree usage
	struct Statistics {
		unsigned int blocksRead; //!< Quantity of blocks read since the tree was initialized
		unsigned int blocksCreated; //!< Quantity of blocks created since the tree was initialized
		unsigned int blocksInDisk; //!< Quantity of blocks stored in disk
	};
	
	//! Returns the BTree usage statistics so far
	/*!
	 * Including the quantity of blocks in the file is optional
	 * because it requires reading the file header to update the field. The
	 * read value will be stored in memory and can be accessed later without
	 * reading the file header again.
	 *
	 * @param includeFileBlockCount True if you want to update the
	 * Statistics::blocksInDisk value, false otherwise
	 *
	 * @return Statistics
	 */
	const Statistics& getStatistics(bool includeFileBlockCount = false) const;
	
	//! Reset all statistics values to 0
	/*! Use with caution */
	void resetStatistics();
	
	//! Updates the header with the total blocks in the file
	/*!
	 * Very important to be used once you've finished using a tree that was
	 * initialized with BTree::create
	 */
	void finishInsertions();
	
private:
	//! File header data
	struct FileHeader {
		long rootAddress;
		unsigned int blockCount;
	};

	//! File header block
	typedef Block<FileHeader, BlockSize> FileHeaderBlock;
	
	//! B-tree node
	struct BNode {
		//! Disk address
		/*!
		 * Ideally, the node before being first written should have its offset
		 * equal to -1. Only after its first writing should it store its true
		 * disk offset.
		 */
		long offset;
		bool isLeaf; //!< True if the node is a leaf, false if it's an internal node
		std::size_t size; //!< Quantity of values currently stored in the node
		
		//! Node values
		/*!
		 * Most of the time, the node will only be using the first `2M`
		 * positions of this array. The additional space for 1 value is used
		 * temporarily to deal with overflows during BTree::insert.
		 */
		T values[2 * M + 1];
		
		//! Os apontadores de nó.
		/*!
		 * Most of the time, the node will only use the first `2M+1` positions
		 * of this array. The additional space for 1 value is used temporarily
		 * to deal with overflows during BTree::insert.
		 */
		long children[2 * M + 2];
		
		//! Initializes the node
		/*!
		 * The value of BNode::offset is initialized as -1 so that we can know
		 * whether the node has already been written to the file or not.
		 *
		 * This method must always be called upon declaring a BNode unless the
		 * node will be read through BTree::readFromDisk.
		 *
		 * BNode's initialization was implemented as a method rather than a
		 * constructor because BNode must be a POD in order to be read and
		 * written in the file.
		 *
		 * @param isLeaf True if the node will be a leaf, false if it will be an internal node
		 * @param size Initial node size
		 */
		void initialize(bool isLeaf = true, int size = 0);
		
		//! Checks if the node has reached its maximum capacity
		/*!
		 * @return True if BNode::size >= 2M, false otherwise
		 */
		bool isFull() const;
		
		//! Seeks a value that's equivalent to the one provided
		/*!
		 * Seeks the value throughout the file
		 *
		 * @tparam U See BTree::seek
		 *
		 * @param key Value to seek
		 * @param tree BTree to access the file and statistics
		 *
		 * @return Pointer with the value if found, null pointer otherwise
		 */
		template <typename U>
		std::unique_ptr<T> seek(const U& key, BTree& tree) const;
	};

	//! Node block
	typedef Block<BNode, BlockSize> BNodeBlock;

	std::FILE *m_file; //!< File pointer to the file where data will be stored
	BNodeBlock m_root; //!< Root node of the B-tree
	mutable Statistics m_stats; //!< Where BTree stores its read and write statistics
	
	//! Retorna um bloco com o nó lido no offset fornecido.
	/*!
	 * Assumes that the provided offset will always be valid. If an invalid
	 * offset is provided, the result is undefined.
	 *
	 * @param offset Node's offset in the file
	 *
	 * @return Block with the node found in the provided offset
	 */
	BNodeBlock readFromDisk(long offset);
	
	//! Writes the node to disk
	/*!
	 * If the node still hasn't been written to disk (BNode::offset == -1),
	 * it'll be appended to the end of the file and BNode::offset will be
	 * updated. In case this happens, Statistics::blocksCreated will be
	 * incremented.
	 *
	 * If the node has already been written to disk it'll simply be updated.
	 *
	 * @param node Node to write
	 */
	void writeToDisk(BNodeBlock& node);
	
	//! Reads the file header
	/*!
	 * This method assumes that BTree has already been initialized and the file
	 * is open. Otherwise, behavior is undefined.
	 *
	 * Increments Statistics::blocksRead.
	 *
	 * @return Block with the file header
	 */
	FileHeaderBlock readHeader() const;
	
	//! Updates the file header in disk
	/*!
	 * @param header Block with file header to write
	 */
	void writeHeader(const FileHeaderBlock& header);
	
	//! Auxiliary struct with results from an insertion overflow
	struct OverflowResult {
		T middle; //!< The value that, after splitting nodes, shall be used as the middle value and must be inserted in the parent node
		long rightNode; //!< Offset of the node that'll have to be the pointer to the right of the OverflowResult::middle value
	};
	
	//! Internal method for B-tree insertion
	/*!
	 * If the node is a leaf, it seeks the node pointer for the node in which
	 * the value must be inserted and tries to insert again recursively. If the
	 * node isn't a leaf, inserts normally.
	 *
	 * In case of overflow, the method will split the node provided as
	 * parameter and return an OverflowResult instance with data for the parent
	 * node to deal with the overflow. The parent will try to insert again but,
	 * this time, in itself.
	 *
	 * The value of rightNodeOffset should be different from -1 only if the
	 * method is dealing with an overflow. In this case, it's the parent node
	 * which must be trying to insert the node.
	 *
	 * @param node Node in which to insert
	 * @param value Value to insert
	 * @param rightNodeOffset Offset of the node to the right of the value to
	 * insert; if -1, the node to the right doesn't exist yet
	 *
	 * @return A pointer with an OverflowResult instance in case of overflow,
	 * null otherwise
	 */
	std::unique_ptr<OverflowResult> insert(BNodeBlock& node, T value, long rightNodeOffset = -1);
};

#include "BTree.inl"

#endif // _BTREE_HPP_INCLUDED_
