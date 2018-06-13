#ifndef _IDEALBTREE_HPP_INCLUDED_
#define _IDEALBTREE_HPP_INCLUDED_

#include "BTree.hpp"

//! Auxiliary function to calculate the max order of a BTree to store T-type values
/*!
 * The calculation is done based on BlockSize, the result being the max order
 * you can have for your tree without having your node exceed BlockSize in
 * bytes.
 *
 * We arrived at it through the following initial formula:
 *
 * `BlockSize = sizeof(long) + sizeof(bool) + sizeof(std::size_t) + (2 * M + 1) * sizeof(T) + (2 * M + 2) * sizeof(long)`
 *
 * This formula takes into account the size of members of a BTree::BNode.
 *
 * @tparam T Type that will be stored in BTree
 * @tparam BlockSize Size in bytes
 *
 * @return Calculation result
 */
template <typename T, unsigned int BlockSize>
constexpr auto maxBTreeOrder() {
	constexpr auto c = 3 * sizeof(long) + sizeof(bool) + sizeof(std::size_t) + sizeof(T);
	static_assert(c < BlockSize, "B-tree order will be negative, consider increasing blockSize");

	constexpr auto M = (BlockSize - c) / (2 * (sizeof(T) + sizeof(long)));
	static_assert(M >= 1, "Type T too big, consider increasing blockSize");

	return M;
}

//! BTree with ideal pre-calculated M order based on `sizeof(T)`
/*!
 * The nodes of such a BTree will be making the most out of their block space
 * 
 * @tparam T Type to be stored
 * @tparam BlockSize %Block size in bytes
 */
template <typename T, unsigned int BlockSize = BLOCK_SIZE>
using IdealBTree = BTree<T, maxBTreeOrder<T, BlockSize>(), BlockSize>;

#endif //_IDEALBTREE_HPP_INCLUDED_
