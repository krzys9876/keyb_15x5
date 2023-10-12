template<class T>
class ArrList {
  public:

  ArrList(int nMaxSize);
  ~ArrList();

  void setVal(int pos,T value);
  T getVal(int pos);

  bool insertVal(T value);
  bool removeVal(T value);
  void empty();
  int getSize();
  bool isEmpty();
  int getPos(T value);
  bool notExists(T value);

  private: 
  int maxSize;
  int currSize;
  T *arr;
  T *emptyElem;
};

// implementacja w pliku .h - konieczna dla szablonu template<class T>

template<class T>ArrList<T>::ArrList(int nMaxSize) {
  maxSize=nMaxSize;
  arr=new T[maxSize];
  emptyElem=new T;
  empty();
}
template<class T>ArrList<T>::~ArrList() {
  delete [] arr;
  delete emptyElem;
}
template<class T> void ArrList<T>::setVal(int pos,T value) { arr[pos]=value;}
template<class T> T ArrList<T>::getVal(int pos) {
  if(pos>=currSize) return *emptyElem;
  else return arr[pos];
}
template<class T> bool ArrList<T>::insertVal(T value) {
  if(currSize>=maxSize) return false;
  else {
    setVal(currSize,value);
    currSize++;
    return true;
  }
}
template<class T> void ArrList<T>::empty() { currSize=0;}
template<class T> int ArrList<T>::getSize() { return currSize;}
template<class T> bool ArrList<T>::isEmpty() { return currSize==0;}
template<class T> int ArrList<T>::getPos(T value) {
  for(int i=0;i<currSize;i++) if(value==arr[i]) return i;
  return -1;
}
template<class T> bool ArrList<T>::notExists(T value) { return getPos(value) == -1; }
template<class T> bool ArrList<T>::removeVal(T value) {
  int pos=getPos(value);
  if(pos>-1) {
    for(int i=pos;i<currSize-1;i++) setVal(i,getVal(i+1));
    currSize--;
    return true;
  } else return false;
}
