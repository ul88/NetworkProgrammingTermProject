#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <QSharedPointer>
#include <functional>

template<typename T>
class LinkedList
{
public:
    LinkedList()
        : m_size(0), node{nullptr}
    {
    }

    void insertBack(const T& data){
        QSharedPointer<NodeType> newNode = QSharedPointer<NodeType>::create();

        newNode->element = data;
        newNode->next = nullptr;

        insertBackImpl(newNode);
    }

    void inesrtBack(T&& data){
        QSharedPointer<NodeType> newNode = QSharedPointer<NodeType>::create();

        newNode->element = std::move(data);
        newNode->next = nullptr;

        insertBackImpl(newNode);
    }

    // insert 함수는 기본적으로 index번째에 삽입
    // insert(data, 2);를 진행하면
    // 0이면 첫번쨰
    // 1이면 두번째
    // 2이면 세번째
    // 따라서, 0, 1, 2, 3 이렇게 4개의 값이 있을 때 위 함수를 진행하여 5를 추가하면
    // 0, 1, 5, 2, 3으로 변화
    // index값을 요소 개수보다 많게 정하면 강제로 맨 뒤에 삽입
    void insert(T&& data, int index){
        if(index > m_size) {
            insertBack(data);
            return;
        }

        auto newNode = QSharedPointer<NodeType>::create();
        newNode->element = std::move(data);
        newNode->next = nullptr;

        insertImpl(newNode, index);
    }

    void insert(const T& data, int index){
        if(index > m_size) {
            insertBack(data);
            return;
        }

        auto newNode = QSharedPointer<NodeType>::create();
        newNode->element = data;
        newNode->next = nullptr;

        insertImpl(newNode, index);
    }

    int getSize() const { return m_size; }

    const T get(int index) {
        int i=-1;
        for(auto head = node; head ; head = head->next){
            if(index != ++i) continue;
            return head->element;
        }
        return nullptr;
    }

    // search 조건은 compare 함수를 정의함으로써 사용할 수 있다.
    // 모든 요소에 대해서 compare 함수를 실행하여 true가 되는 위치를 반환
    template<typename Compare>
    int search(Compare compare){
        int index = 0;
        for(auto head = node; head; head = head->next){
            if(compare(head->element)){
                return index;
            }
            ++index;
        }
        return index;
    }
private:
    struct NodeType{
        T element;
        QSharedPointer<NodeType> next;
    };

    QSharedPointer<NodeType> getTail(){
        QSharedPointer<NodeType> head = node;
        while(head->next){
            head = head->next;
        }
        return head;
    }

    void insertBackImpl(QSharedPointer<NodeType> newNode){
        if(!node){
            node = newNode;
            m_size++;
            return;
        }

        QSharedPointer<NodeType> tail = getTail();
        tail->next = newNode;
        m_size++;
    }

    void insertImpl(QSharedPointer<NodeType> newNode, int index){
        if(!node && index == 0){
            node = newNode;
            m_size++;
            return;
        }

        if(index == 0){
            newNode->next = node;
            node = newNode;
            m_size++;
            return;
        }

        int i = 0;
        for(auto head = node; head; head = head->next){
            if(index != ++i) continue;
            newNode->next = head->next;
            head->next = newNode;
            m_size++;
            return;
        }
    }

    QSharedPointer<NodeType> node;
    int m_size;
};

#endif // LINKEDLIST_H
