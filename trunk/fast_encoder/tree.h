template <class T>
struct Tree {
public:
	//interface declarations
	class Node;
	 
	Tree() {};
	Tree(const Tree&) {};
	Tree(const T d) {rootNode = new Node(d);};
	Tree& operator=(const Tree&) {};
	~Tree() {};
	Node* get_rootNode() const {return rootNode;};
	long get_totalCount() const {return totalCount;};
	void set_totalCount(const long val) {totalCount = val;};
	void inc_totalCount() {++totalCount;}

	class iterator;

	class Node {
	public:
		//interface declarations
		Node() {};
		Node(const Node &n) {};
		Node(const T d)
			: selfCount(1), leftCount(0), data(d), left(0), right(0) {};
		Node& operator=(const Node&) {};
		~Node() {};
		Node* createChild(const T d)
		{
			if(data > d) return (left = new Tree<T>::Node(d));
			else return (right = new Tree<T>::Node(d));
		};
		short get_selfCount() const {return selfCount;};
		long get_leftCount() const {return leftCount;};
		void set_selfCount(const short val) {selfCount = val;};
		void set_leftCount(const long val) {leftCount = val;};

	private:
		//private declarations
		short selfCount; //short?!
		long leftCount;
		Node *left, *right;
		T data;
	};

	class iterator {
	public:
		//interface declarations
		iterator() {};
		iterator(Tree *t, long rc)
			: node(t->get_rootNode()), rightCount(rc) {t->inc_totalCount();};
		iterator(const iterator &it)
			: node(it.get_node()), rightCount(it.get_rightCount()) {};
		~iterator() {};
		iterator operator[](const T d)
		{
			//static Node *next_node; // multithreading?
			while(node->data != d)
			{
				if(node->data > d)
				{
					node->inc_leftCount;
					if(node->left) 
					{
						node = node->left;
					}
					else node = node->createChild(d);
				}
				else
				{
					if(node->right) 
					{
						node = node->right;
					}
					else node = node->createChild(d);
				}
			}
			node->inc_selfCount();
			return *this;
		};

		short get_selfCount() const {return node->get_selfCount();};
		long get_leftCount() const {return node->get_leftCount();};
		void set_selfCount(const short val) {node->set_selfCount(val);};
		void set_leftCount(const long val) {node->set_leftCount(val);};
		void inc_selfCount() {node->set_selfCount(node->get_selfCount() + 1);};
		void inc_leftCount() {node->set_leftCount(node->get_leftCount() + 1);};
		Node* get_node() {return node;};

	private:
		//private declarations
		Node *node;
		long rightCount;
	};

private:
	//private declarations
	Node *rootNode;
	long totalCount;
};