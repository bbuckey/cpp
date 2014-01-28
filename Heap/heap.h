        #include "dsexceptions.h"
        #include "vector.h"


        template <class Comparable>
        class BinaryHeap
        {
          public:
            explicit BinaryHeap( int capacity = 100 );

            bool isEmpty( ) const;
            bool isFull( ) const;
            const Comparable & findMin( ) const;

            void insert( const Comparable & x );
            void deleteMin( );
            void deleteMin( Comparable & minItem );
            void makeEmpty( );

          private:
            int                currentSize;  // Number of elements in heap
            vector<Comparable> array;        // The heap array

            void buildHeap( );
            void percolateDown(int hole);
        };
