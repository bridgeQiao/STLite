#pragma once

#include "jw_iterator.hpp"
#include "jw_alloc.hpp"
#include "jw_construct.hpp"

namespace jw {

	using __rb_tree_color_type = bool;
	const __rb_tree_color_type __rb_tree_red = 0;
	const __rb_tree_color_type __rb_tree_black = 1;

	template<typename T>
	struct __rb_tree_node {
		using color_type = __rb_tree_color_type;
		using link_type = __rb_tree_node<T>*;

		static link_type minimum(link_type x) {
			while (x->left != 0)
				x = x->left;
			return x;
		}

		static link_type maximum(link_type x) {
			while (x->right != 0)
				x = x->right;
			return x;
		}

		color_type color;
		link_type parent;
		link_type left;
		link_type right;
		T value_field;
	};

	template<typename T, typename Ref, typename Ptr>
	struct __rb_tree_iterator {
		using value_type = T;
		using reference = Ref;
		using pointer = Ptr;
		using iterator = __rb_tree_iterator<T, T*, T&>;
		using const_iterator = __rb_tree_iterator<T, const T*, const T&>;
		using Self = __rb_tree_iterator<T, Ref, Ptr>;
		using link_type = __rb_tree_node<T>*;

		explicit __rb_tree_iterator(link_type x) : node(x) {}
		__rb_tree_iterator(const iterator& rhs) { node = rhs.node; }

		// ++, --
		Self& operator++() {
			if (node->right != 0) {
				node = node->right;
				while (node->left != 0)
					node = node->left;
			}
			else {
				link_type y = node->parent;
				while (y->right == node) {
					node = y;
					y = node->parent;
				}
				// except: when root hasn't right child, node->right == y, 
				//         node == header
				if (node->right != y)
					node = y;
			}
			return *this;
		}
		Self operator++(int) {
			Self tmp = *this;
			++* this;
			return tmp;
		}

		Self& operator--() {
			// iterator == end()
			if (node->color == __rb_tree_red && node->parent->parent == node)
				node = node->right;
			else if (node->left != 0) {
				node = node->left;
				while (node->right != 0)
					node = node->right;
			}
			else {
				link_type y = node->parent;
				while (y->left == node) {
					node = y;
					y = node->parent;
				}
				node = y;
			}
			return *this;
		}
		Self operator--(int) {
			Self tmp = *this;
			--* this;
			return tmp;
		}

		// *, ->
		reference operator*() const { return node->value_field; }
		pointer operator->() const { return &(operator*()); }
		bool operator==(const Self& rhs) const { return node == rhs.node; }
		bool operator!=(const Self& rhs) const { return node != rhs.node; }

		link_type node;
	};

	// rotate
	template<typename T>
	inline void __rb_tree_rotate_left(__rb_tree_node<T>* x, __rb_tree_node<T>*& root) {
		__rb_tree_node<T>* y = x->right;
		// set left and right
		x->right = y->left;
		if (y->left != 0)
			y->left->parent = x;
		y->left = x;
		// set parent
		y->parent = x->parent;
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		x->parent = y;
	}

	template<typename T>
	inline void __rb_tree_rotate_right(__rb_tree_node<T>* x, __rb_tree_node<T>*& root) {
		__rb_tree_node<T>* y = x->left;
		// set left and right
		x->left = y->right;
		if (y->right != 0)
			y->right->parent = x;
		y->right = x;
		// set parent
		y->parent = x->parent;
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		x->parent = y;
	}

	template<typename T>
	inline void __rb_tree_rebalance(__rb_tree_node<T>* x, __rb_tree_node<T>*& root) {
		x->color = __rb_tree_red;
		while (x != root && x->parent->color == __rb_tree_red) { // exit conflict
			if (x->parent == x->parent->parent->left) { // P at left side
				__rb_tree_node<T>* s = x->parent->parent->right;
				if (s != 0 && s->color == __rb_tree_red) { // P and S are red
					x->parent->color = __rb_tree_black;
					s->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else { // S is null or black
					if (x == x->parent->right) {
						x = x->parent; // after rotate, x become parent node
						__rb_tree_rotate_left(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else { // P at right side
				__rb_tree_node<T>* s = x->parent->parent->left;
				if (s != 0 && s->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					s->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->left) {
						x = x->parent;
						__rb_tree_rotate_right(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}

		// set root color
		root->color = __rb_tree_black;
	}

	// according to https://blog.csdn.net/m0_37589327/article/details/78518324
	template<typename T>
	__rb_tree_node<T>* __rb_tree_rebalance_for_erase(__rb_tree_node<T>* z,
		__rb_tree_node<T>*& root, __rb_tree_node<T>*& leftmost, __rb_tree_node<T>*& rightmost) {
		__rb_tree_iterator<T, T&, T*> y(z);
		if (z->left != 0 && z->right != 0) { // has two child
			y.node = z->right;
			++y;
			z->value_field = y.node->value_field;
			__rb_tree_rebalance_for_erase(y.node, root, leftmost, rightmost);
		}
		else { // has one or null child
			if (z == root) {
				if (z->left != 0) { // only left child
					z->left->parent = root->parent;
					root = z->left;
					root->color = __rb_tree_black;
				}
				else if (z->right != 0) { // only right child
					z->right->parent = root->parent;
					root = z->right;
					root->color = __rb_tree_black;
				}
				else { // null
					leftmost = root->parent;
					rightmost = root->parent;
					root = 0;
				}
				return z;
			}
			else { // z != root
				// leftmost and rightmost
				if (z == rightmost) {
					y.node = z;
					--y;
					rightmost = y.node;
				}
				if (z == leftmost) {
					y.node = z;
					++y;
					leftmost = y.node;
				}

				// one child, must be red
				if (z->left != 0) {
					z->left->parent = z->parent;
					if (z->parent->left == z)
						z->parent->left = z->left;
					else
						z->parent->right = z->left;
					z->left->color = __rb_tree_black;
				}
				else if (z->right != 0) {
					z->right->parent = z->parent;
					if (z->parent->left == z)
						z->parent->left = z->right;
					else
						z->parent->right = z->right;
					z->right->color = __rb_tree_black;
				}
				else { // null child
					__rb_tree_node<T>* replace = z;
					// color is red, do nothing
					while (replace->color != __rb_tree_red && replace != root) {
						// color is black, must has brother node
						__rb_tree_node<T>* p = replace->parent;
						__rb_tree_node<T>* bro = p->left == replace ? 
							p->right : p->left;

						// if bro is red, p and bro's chlid must be black, 
						// change to p is black
						if (bro->color == __rb_tree_red) {
							if (p->left == bro)
								__rb_tree_rotate_right(p, root);
							else
								__rb_tree_rotate_left(p, root);
							p->color = __rb_tree_red;
							bro->color = __rb_tree_black;
							bro = p->left == replace ? p->right : p->left;
						}

						// bro is black, case 1: bro hasn't child
						if ((bro->left == 0 || bro->left->color == __rb_tree_black) &&
							(bro->right == 0 || bro->right->color == __rb_tree_black)) {
							if (p->color == __rb_tree_red) {
								p->color = __rb_tree_black;
								bro->color = __rb_tree_red;
								break;
							}
							else {
								bro->color = __rb_tree_red;
								// rebalance parent node, as black leaf node
								replace = p;
							}
						}
						else {
							if (bro->left != 0 && bro->left->color == __rb_tree_red) {
								__rb_tree_rotate_right(bro, root);
								bro->parent->color = p->color;
								p->color = __rb_tree_black;
								if (p->left == replace)
									__rb_tree_rotate_left(p, root);
								else
									__rb_tree_rotate_right(p, root);
							}
							else if (bro->right != 0 && bro->right->color == __rb_tree_red) {
								bro->right->color = __rb_tree_black;
								bro->color = p->color;
								p->color = __rb_tree_black;
								if (p->left == replace)
									__rb_tree_rotate_left(p, root);
								else
									__rb_tree_rotate_right(p, root);
							}
							break;
						}
					}

					// set pointer to 0
					if (z->parent->left == z)
						z->parent->left = 0;
					else
						z->parent->right = 0;
				}
			}
		}
	}

	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Alloc = alloc>
		class rb_tree {
		protected:
			using rb_tree_node = __rb_tree_node<Value>;
			using rb_tree_node_allocator = simple_alloc<rb_tree_node, alloc>;
			using color_type = __rb_tree_color_type;
		public:
			using key_type = Key;
			using value_type = Value;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using link_type = rb_tree_node*;
			using size_type = size_t;
			using difference_type = ptrdiff_t;
			using iterator = __rb_tree_iterator<value_type, reference, pointer>;
			using const_iterator = __rb_tree_iterator<value_type, const const_reference, const_pointer>;

		protected:
			// allocate and deallocate function
			link_type get_node() { return rb_tree_node_allocator::allocate(); }
			void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

			// create, clone and destroy
			link_type create_node(const value_type& value) {
				link_type tmp = get_node();
				construct(&tmp->value_field, value);
				return tmp;
			}
			link_type clone_node(link_type rhs) {
				link_type tmp = create_node(rhs->value_field);
				tmp->color = rhs->color;
				tmp->left = 0;
				tmp->right = 0;
			}
			void destroy_node(link_type p) {
				destroy(&p->value_field);
				put_node(p);
			}

			// some useful function
			const key_type& key(link_type x) {
				return KeyOfValue()(x->value_field);
			}
			link_type& root() const { return header->parent; }
			link_type& leftmost() const { return header->left; }
			link_type& rightmost() const { return header->right; }

			static link_type minimum(link_type x) { return __rb_tree_node::minimum(x); }
			static link_type maximum(link_type x) { return __rb_tree_node::maxmimum(x); }

			void init() {
				header = get_node();
				header->color = __rb_tree_red;
				header->parent = 0;
				header->left = header;
				header->right = header;
				// not initialize header->value_field!!!
			}

		private:
			iterator __insert(link_type x, link_type y, const value_type& value);
			link_type __copy(link_type x, link_type p);
			void __erase(link_type x);

		public:
			rb_tree() : node_count(0), key_compare(Compare()) { init(); }
			~rb_tree() { clear(); put_node(header); }

			rb_tree& operator=(const rb_tree& rhs);

			iterator begin() { return iterator(leftmost()); }
			const_iterator begin() const { return const_iterator(leftmost()); }
			iterator end() { return iterator(header); }
			const_iterator end() const { return const_iterator(header); }
			size_type size() const { return node_count; }
			bool empty() const { return node_count == 0; }
			void clear();
			Compare key_comp() const { return key_compare; }
			std::pair<iterator, bool> insert_unique(const value_type& value);
			void erase(iterator position);
			iterator find(const key_type& k);

		protected:
			size_type node_count;
			link_type header;
			Compare key_compare;
	};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(link_type x, 
		link_type y, const value_type& value)
	{
		link_type new_node = create_node(value);
		if (y == header || key_compare(KeyOfValue()(value), key(y))) { // insert root or left
			y->left = new_node;
			if (y == header) {
				root() = new_node;
				rightmost() = new_node;
			}
			else if (y == leftmost())
				leftmost() = new_node;
		}
		else { // insert right
			y->right = new_node;
			if (y == rightmost())
				rightmost() = new_node;
		}

		// process new_node
		new_node->parent = y;
		new_node->left = 0;
		new_node->right = 0;

		__rb_tree_rebalance(new_node, root());
		++node_count;
		return iterator(new_node);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p)
	{
		return link_type();
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__erase(link_type x)
	{
		while (x != 0) {
			// delete order: right, root, left
			__erase(x->right);
			link_type y = x->left;
			destroy_node(x);
			x = y;
		}
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
		operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs)
	{
		if (this != &rhs) {
			clear();
			node_count = 0;
			key_compare = rhs.key_compare;
			if (rhs.node_count == 0) {
				root() = 0;
				leftmost() = header;
				rightmost() = header;
			}
			else {
				root() = __copy(rhs.root(), header);
				leftmost() = minimum(root());
				rightmost() = maximum(root());
				node_count = rhs.node_count;
			}
		}
		return *this;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear()
	{
		__erase(root());
		root() = 0;
		leftmost() = header;
		rightmost() = header;
		node_count = 0;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& value)
	{
		link_type y = header;
		link_type x = root();

		// find leaf node
		bool comp = true;
		while (x != 0) {
			y = x;
			comp = key_compare(KeyOfValue()(value), key(x));
			x = comp ? x->left : x->right;
		}

		iterator j = iterator(y);
		if (comp) { // insert root or left child
			if (j == begin())
				return std::pair<iterator, bool>(__insert(x, y, value), true);
			else
				// KeyOfValue(value) less than key(y), but maybe equal to key(--y)
				--j;
		}
		// insert right child
		if (key_compare(key(j.node), KeyOfValue()(value)))
			return std::pair<iterator, bool>(__insert(x, y, value), true);
		// equal
		return std::pair<iterator, bool>(j, false);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position)
	{
		link_type y = __rb_tree_rebalance_for_erase(position.node,
			root(), leftmost(), rightmost());
		destroy_node(y);
		--node_count;
	}
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const key_type& k)
	{
		link_type y = header;
		link_type x = root();
		while (x != 0) {
			if (!key_compare(key(x), k)) {
				y = x;
				x = x->left;
			}
			else {
				x = x->right;
			}
		}

		iterator j(y);
		return (j == end() || key_compare(key(y), k)) ? end() : j;
	}
}