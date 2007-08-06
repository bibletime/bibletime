/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef UTILAUTOPTRVECTOR_H
#define UTILAUTOPTRVECTOR_H

namespace util {

	/**
	 * This class provides a simple vector which works on pointers.
	 * All pointer are deeted at destruction time of an AutoPtrVector object.
	 * This vector uses a single linked list to store the pointers.
	 * This class provides the methods first(), current() and next() for navigation.
	 *
	 * @author Joachim Ansorg
	*/
	template<class T>
	class AutoPtrVector {
public:
		/** Default constructor.
		* The default constructor. This creates an empty vector.
		*/
explicit AutoPtrVector() : m_first(0), m_current(0), m_end(0) {}
		;

		/** Copy constructor using deep copy.
		* This does a deep copy of the passed AutoPtrVector.
		* @param old The vector to be copied.
		*/
AutoPtrVector(AutoPtrVector& old) : m_first(0), m_current(0), m_end(0) {
			this->operator=(old); //share the code with the copy operator
			/*  if (this != &old) {
			   Item* last = m_first;
			   Item* prev = 0;
			   
			   for (T* c = old.first(); c; c = old.next()) {
			    last = new Item( new T(*c) );
			    
			    if (prev) {
			     prev->next = last;
			    }
			 
			    prev = last;
			   }
			  }*/
		};

		AutoPtrVector& operator=(AutoPtrVector& old) {
									//at first delete all items, then copy old into new items
									clear();

									if (this != &old) { //only copy if the two pointers are different
										Item* last = m_first;
										Item* prev = 0;

										for (T* c = old.first(); c; c = old.next()) {
											last = new Item( new T(*c) );

											if (prev) {
												prev->next = last;
											}

											prev = last;
										}
									}

									return *this;
								};

		/** Destructor.
		* Deletes all the objects which belong to the stored pointers
		* @see clear()
		*/
		virtual ~AutoPtrVector() {
			clear();
		};


		/** Append an item
		*
		* Append a new item to this vector.
		*/
		inline void append(T* type) {
			if (!m_first) { //handle the first item special
				m_first = new Item( type );
				m_end = m_first;
			}
			else {
				m_end->next = new Item( type );
				m_end = m_end->next;
			}
		};

		/** The first item of this vector.
		*
		* @return The first item of this vector. Null of there are no items.
		*/
		inline T* const first() const {
			m_current = m_first;
			return m_current ? m_current->value : 0;
		};

		/** The current item.
		*
		* @return The current item reached by first() and next() calls
		*/
		inline T* const current() const {
			return m_current->value;
		};

		/** Moves to the next item.
		*
		* @return Moves to the next item
		*/
		inline T* const next() const {
			if (m_current && m_current->next) {
			m_current = m_current->next;
			return m_current->value;
		}

		return 0;
	};

	/** Returns if this conainer is empty.
	*
	* @return If this vector has items or not. True if there are no items, false if there are any
	*/
	inline const bool isEmpty() const {
			return bool(m_first == 0);
		};

		/** Clear this vector.
		* This deletes all objects which belong to the stored pointers.
		*/
		inline void clear() {
			Item* i = m_first;
			Item* current = 0;

			while (i) {
				delete i->value; //delete the object which belongs to the stored pointer

				current = i;
				i = current->next;
				delete current; //delete the current item after we got the next list item
			}
		};

private:
		/**
		* Our internal helper class to store the pointers in a linked list.
		*/
		struct Item {
Item(T* t = 0) : value(t), next(0) {}
			;

			T* value;
			Item* next;
		};

		mutable Item* m_first;
		mutable Item* m_current;
		mutable Item* m_end;
	};

} //end of namespace


#endif
