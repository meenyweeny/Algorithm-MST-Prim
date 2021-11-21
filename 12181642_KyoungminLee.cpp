#include<iostream>
#include<vector>
#include<cstring>
using namespace std;

//편한 사용을 위한 매크로들
#define MAX 10000 + 1
#define unseen 0
#define fringe -1
#define tree 1
#define used -10
#define empty -11

//최소신장트리 == minimum spanning tree == MST라고 생각하고 코드 및 주석 작성하였습니다

vector<pair<int,int>> graph[MAX]; // {vertex number, weight}를 관리하는 adjacency list graph

int state[MAX]; //vertex들의 방문 상태를 관리할 배열 (tree, fringe, unseen)
int weightMST[MAX]; //decreaseKey를 위해 weight 정보를 저장하고 있을 배열
//0->unseen / used->tree / 10<=n<=10000 -> fringe

//global variables for input commands
int n,m,q; //조형물 정보의 수, 조형물 사이의 산책로 정보의 수, 질의의 수
int s,e,w; //starting vertex, end vertex, weight -> m개 입력받기 위한 변수
char ch;

class myPriorityQueue { //priority queue를 unsorted sequence로 구현
    //priority queue는 weight를 기준으로 min값을 찾음
    //만약 같은 weight가 있다면? -> 더 작은 숫자가 우선순위를 가짐.
    //weight는 weightMST 배열에서 확인 가능.
    
public:
    int sequence[MAX]; //unsorted sequence
    int elements; //내부 원소 수
    int index; //next index
    
    myPriorityQueue(){ //constructor
        elements = 0;
        index = 1; //index는 1부터 시작
    }
    
    void clearPriorityQueue(){ //새로 사용하기 위해 clear하는 과정
        memset(sequence,0,sizeof(sequence)); //배열을 0으로 초기화
        elements = 0; //원소 수 0으로 초기화
        index = 1; //index 시작 1로 초기화
    }
    
    void insert(int k){ //unsorted sequence에 단순히 넣기
        sequence[index] = k; //맨 뒤에 vertex 넣어준다
        ++index; //다음 삽입을 위한 index값 update
        ++elements; //priorityqueue에서의 element값 하나 늘었으니까 +1
    }
    
    pair<int,int> removeMin(){ //가장 weight 작은 vertex와 그의 weight를 return
        //min인걸 찾기 (같은 weight를 가졌다면 수가 더 작은 것 -> 거꾸로 내려오면 된다)
        int minWeight = MAX + 1;
        int minIndex = index-1; //탐색은 index-1로부터 시작한다
        
        for(int i=index-1; i>0; i--){ //배열을 거꾸로 탐색하며 minimum index와 weight 갱신
            if(sequence[i] == empty) continue; //이미 삭제된 칸이면 건너뛴다.
            //if(state[sequence[i]] == tree) continue;
            if(minWeight>=weightMST[sequence[i]]){ //지금의 minWeight보다 작은 값이 있으면 당연히 그로 업데이트
                
                //같거나 작은 경우에서 같거나 / 작은 이 두가지로 세분화하여 minIndex를 구함
                
                if(minWeight == weightMST[sequence[i]]){
                    //만약 minWeight가 나랑 같다면?
                    //vertex 번호가 더 작은게 minIndex가 된다
                    if(sequence[i] < sequence[minIndex]){
                        minIndex = i;
                    }
                }
                else{
                    //minWeight가 다르면? 작은거니까 그냥 바로 minIndex 업데이트한다
                    minIndex = i;
                }
                
                //minWeight는 같거나 작거나 update해준다 (결국 같은 결과)
                minWeight = weightMST[sequence[i]];
            }
        }
        
        int minVertex = sequence[minIndex]; //몇 번 vertex가 추가될까?
        
        //비워주는 과정 (비었다고 표시해주고, weight가 min으로 잡히지 않게끔 해주고.)
        sequence[minIndex] = empty;
        weightMST[minVertex] = MAX+1;
        
        --elements; //element 수는 하나 줄여준다
        
        return {minVertex,minWeight}; //vertex,weight의 pair 형으로 return
    }

};

class MinimumSpanningTree { //minimum spanning tree를 prim 알고리즘으로 찾는 과정
public:
    vector<int> myMST; //MST를 만드는 순서를 저장할 vector
    int weight; //MST의 weight를 저장할 integer형 변수 weight (-> 매번 만들 때마다 이 값은 같을 것이다)
    myPriorityQueue manageVertex; //MST를 만드는 과정에서, vertex들의 최소값 정보를 관리할 PQ
    MinimumSpanningTree(){ //constructor
        weight=0;
    }
    
    void clearMST(){ //새로 시작할 때 MST의 모든 정보를 비우기 위해 호출해줄 clear function
        manageVertex.clearPriorityQueue(); //PQ class의 clear function 불러온다
        myMST.clear(); //MST 생성 순서를 담은 vertex를 clear한다
        weight = 0; //weight 또한 0으로 clear한다
    }
    
    void printMST(){ //출력함수
        cout<<weight<<" "; //MST의 weight를 출력
        
        for(auto i : myMST){ //생성 순서대로 MST에 저장해놓은 vertex number들을 출력
            cout<<i<<" ";
        }
        cout<<"\n"; //엔터 출력
    }
    
    void decreaseKey(int vertex,int weight){
        //decrease key를 해야하는 상황이면 한다.
        if(weightMST[vertex] > weight){ //해야하는 상황
            //원래 있던 값과, 새로운 값을 비교
            weightMST[vertex] = weight;
        }
    }

    void makeMST(int start){ //MST를 만드는 과정 (입력받은 vertex로부터 시작)
        int loop = n; //while문을 n번 돌아야 하므로
        manageVertex.insert(start); //시작 vertex를 PQ에 넣고 시작한다
        
        pair<int,int> top;
        int topVertex, topWeight;
        int vertexTmp, weightTmp;
        
        while(loop>0){ //MST엔 graph의 모든 vertex가 다 들어있다
            //그러므로 n번 loop를 돈다
            --loop;
            
            top = manageVertex.removeMin(); //가장 작은 값을 삭제하고, return값으로 받아주었다.
            
            topVertex = top.first;
            topWeight = top.second;
            
            //생성 순서 vector에 push한다
            myMST.push_back(topVertex);
            //weight 값을 update한다 (이 또한 removeMin으로부터 받아왔다)
            weight = weight + topWeight;
            
            state[topVertex] = tree; //MST의 일원이 되었으니까 tree vertex로 update
            
            for(auto i : graph[topVertex]){ //vertex에 딸린 것들을 인접리스트에서 탐색하면서 상태에 따라 PQ에 넣을지 건너뛸지 등등을 판단하는 부분이다
                
                vertexTmp = i.first;
                weightTmp = i.second;
                
                if(state[vertexTmp] == tree){
                    //나와 연결된 vertex가 이미 mst의 tree vertex면 그냥 넘어간다
                    continue;
                }
                else if(state[vertexTmp] == fringe){
                    //fringe vertex면, 값을 비교하여 decreaseKey를 필요에 따라 한다
                    decreaseKey(vertexTmp, weightTmp);
                }
                else if(state[vertexTmp] == unseen){
                    //unseen vertex면, fringe로 만들어주고,
                    state[vertexTmp] = fringe;
                    //바로 weight값이 생긴다
                    weightMST[vertexTmp] = weightTmp;
                    //PQ에 그 vertex의 정보 또한 넣어준다.
                    manageVertex.insert(vertexTmp);
                }
            }
        }
    }
};

int main(){
    cin.tie(NULL);
    cout.tie(NULL);
    ios_base::sync_with_stdio(false);
    
    cin>>n>>m>>q;
    
    MinimumSpanningTree myPark; //공원 관리를 하기 위해 MST class 하나 선언
    //이 class에서는, 입력받은 인접리스트 그래프를 바탕으로 최소신장트리를 만들어 줄 것이다.
    
    while(m--){
        cin>>s>>e>>w; //starting vertex, end vertex, weight
        
        //adjacency list 형태로 관리한다
        //시작, 끝 vertex에 서로 {반대편 vertex, 그까지의 edge의 weight} 의 pair 형태로 저장해준다.
        graph[s].push_back({e,w});
        graph[e].push_back({s,w});
    }
    
    while(q--){ //입력받은 질의 수 만큼 MST 생성을 시작
        cin>>ch; //의미없긴하다
        cin>>s;
        
        if(ch=='P'){
            myPark.makeMST(s); //입력한 starting vertex로 부터, MST를 만드는 과정
            myPark.printMST(); //완성한 MST를 print하는 과정
            //clear function - 다른 시작 정점으로부터 하기 위해서
            myPark.clearMST();
            //다른 전역변수들 또한 clear해준다.
            memset(state, unseen, sizeof(state));
            memset(weightMST, 0,sizeof(weightMST));
        }
    }
}
