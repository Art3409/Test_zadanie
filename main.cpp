#include <iostream>
#include <sstream>
#include <fstream>
#include <locale>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <queue>
#include <regex>
using namespace std;

struct Time {
  int hours;
  int minutes;
};
struct Event{
    Time time_event;
    int identifier_event;
    string body;
    int table;
};
struct Computer{
    Time startTimeUse;
    int allTimeUse;
    bool occupation;
    string client;
};
bool isValidTimeFormat(const string& timeStr) {
    if (timeStr.find(':') == string::npos) {
        return false;
    }
    string hoursStr = timeStr.substr(0, 2);
    string minutesStr = timeStr.substr(3, 2);
    int hours, minutes;
    try {
        hours = stoi(hoursStr);
        minutes = stoi(minutesStr);
    } catch (const std::invalid_argument& e) {
        return false;
    }

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return false;
    }

    return true;
}
bool isValidName(const string& name) {
    regex nameRegex("^[a-z0-9_-]+$");
    return regex_match(name, nameRegex);
}
queue<string> processing_function(Event event, Computer* computers, vector<string>& clients, int numTables, queue<string> queueClients){
    bool flagClient=false;
    bool flagFree=false;
    int comp=0;
    if(event.identifier_event == 1){
        for (string clientName : clients) {
            if( clientName == event.body) {
                flagClient=true;
                cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 13 YouShallNotPass" << endl;
            }
        }
        if (flagClient == false){
            clients.push_back(event.body);
            //cout <<"Прим "<< eventBody<<endl;
        }
    }
    if(event.identifier_event == 2){
        for (string clientName : clients) {
            if( clientName == event.body) {
                flagClient=true;
            }
        }
        if(flagClient == true){
            if(computers[event.table-1].occupation == true){
                cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 13 PlaceIsBusy" << endl;
            }
            else{
                computers[event.table-1].startTimeUse = event.time_event;
                computers[event.table-1].occupation = true;
                computers[event.table-1].client = event.body;
            }
        }
        else{
            cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 13 ClientUnknown" << endl;
        }
    }
    if(event.identifier_event == 3){
        for(int i=0; i<numTables;i++){
            if (computers[i].occupation == false){
                flagFree=true;
            }
        }
        if(flagFree == true){
            cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 13 ICanWaitNoLonger!" << endl;
        }
        else{
            if(queueClients.size() > numTables){
                cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 11 " << event.body << endl;
                clients.erase(remove(clients.begin(), clients.end(), event.body), clients.end());
            }
            else{
                queueClients.push(event.body);
            }
        }
    }
    if(event.identifier_event == 4){
        for (string clientName : clients) {
            if( clientName == event.body) {
                flagClient=true;
            }
        }
        if(flagClient == true){
            for(int i=0; i<numTables; i++){
                if(computers[i].client == event.body){
                    comp=i;
                }
            }
            //cout<<"COMP "<<comp<<" "<<computers[comp].client<<" "<<computers[comp].occupation<<" "<<computers[comp].allTimeUse<<".."<<endl;
            computers[comp].occupation=false;
            computers[comp].client="none";
            computers[comp].allTimeUse=computers[comp].allTimeUse + (event.time_event.hours*60 + event.time_event.minutes) - (computers[comp].startTimeUse.hours*60 + computers[comp].startTimeUse.minutes);
            //cout<<"COMP "<<comp<<" "<<computers[comp].client<<" "<<computers[comp].occupation<<" "<<computers[comp].allTimeUse<<".."<<queueClients.front()<<endl;
            clients.erase(remove(clients.begin(), clients.end(), event.body), clients.end());
            if (!queueClients.empty()){
                computers[comp].client=queueClients.front();
                queueClients.pop();
                computers[comp].occupation=true;
                computers[comp].startTimeUse=event.time_event;
                cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 12 " << computers[comp].client <<" "<<comp+1<< endl;
            }
        }
        else{
            cout << setfill('0') << setw(2) << event.time_event.hours << ":" << setfill('0') << setw(2) << event.time_event.minutes << " 13 ClientUnknown" << endl;
        }
    }
    return queueClients;
}
int main(int argc, char *argv[]) {
    setlocale(LC_CTYPE, "rus");
    if (argc != 2){
        cerr << "The nuber of arguments is incorrect!" << endl;
        return 1; 
    }
    string fileName = argv[1];
    ifstream input1(fileName);
    if (!input1) {
        cout << "->File did not open!<-" << endl;
        return 1;
    }
    string error;
    vector<string> clientNames;
    int numTables=0;
    if (!(input1 >> numTables) || numTables <= 0) {
        cerr << "->Format error! The number of tables must be positiv and integer<-" << endl;
        return 1;
    }
    Computer* computersMass;
    computersMass=new Computer[numTables];
    for(int i=0;i<numTables;i++){
        computersMass[i].occupation=false;
        computersMass[i].allTimeUse=0;
    }
    Time startTime, endTime;
    queue<string> queueClients1;
    int costPerHour;
    string startTimeStr, endTimeStr;
    input1 >> startTimeStr >> endTimeStr;
    if (!isValidTimeFormat(startTimeStr) || !isValidTimeFormat(endTimeStr)) {
        cout << numTables<< endl;
        cerr << "->Format error! Invalid time format.<-" << endl;
        return 1;
    }
    startTime.hours = stoi(startTimeStr.substr(0, 2));
    startTime.minutes = stoi(startTimeStr.substr(3, 2));
    endTime.hours = stoi(endTimeStr.substr(0, 2));
    endTime.minutes = stoi(endTimeStr.substr(3, 2));

    bool flagClient=false;
    if (!(input1 >>costPerHour) || costPerHour <= 0) {
        cout << numTables<< endl << setfill('0') << setw(2) << startTime.hours << ":" << setfill('0') << setw(2)<< startTime.minutes<< ' ' << setfill('0') << setw(2) << endTime.hours << ":" << setfill('0') << setw(2)<< endTime.minutes << endl;
        cerr << "->Format error! The cost of hour must be positiv and integer.<-" << endl;
        return 1;
    }
    cout << setfill('0') << setw(2) << startTime.hours << ":" << setfill('0') << setw(2)<< startTime.minutes << endl;
    string eventTimeStr;
    Time eventTime;
    Event eventNew;
    int eventID=0;
    string eventBody;
    int eventTable=0;
    while (!input1.eof()){
        input1 >> eventTimeStr;
        if (!isValidTimeFormat(eventTimeStr)) {
            cerr << "->Format error! Invalid time format.<-" << endl;
            return 1;
        }
        if (!(input1 >> eventID) || eventID <= 0 || eventID > 4 ) {
            cerr << "->Format error! The event IDs must be positiv and integer<-" << endl;
            return 1;
        }
        input1 >> eventBody;
        if (!isValidName(eventBody)) {
            cerr << "->Format error! The name with invalid characters<-" << endl;
            return 1;
        }
        if (eventID==2){
            if (!(input1 >> eventTable) || eventTable <= 0) {
                cerr << "->Format error! The number of table must be positiv and integer<-" << endl;
                return 1;
            }
        }
        for (string clientName : clientNames) {
            if( clientName == eventBody) {
                flagClient=true;
            }
        }
        flagClient=false;
        eventTime.hours = stoi(eventTimeStr.substr(0, 2));
        eventTime.minutes = stoi(eventTimeStr.substr(3, 2));
        cout << setfill('0') << setw(2) << eventTime.hours << ":" << setfill('0') << setw(2) << eventTime.minutes <<" " << eventID << " " << eventBody;
        eventNew.time_event = eventTime;
        eventNew.identifier_event = eventID;
        eventNew.body = eventBody;
        if (eventID == 2){
            cout << " " << eventTable << endl;
            eventNew.table = eventTable;
        }
        else{
            cout << endl;
        }
        if(eventNew.time_event.hours < startTime.hours or eventNew.time_event.hours >= endTime.hours){
            cout << setfill('0') << setw(2) << eventNew.time_event.hours << ":" << setfill('0') << setw(2) << eventNew.time_event.minutes << " 13 NotOpenYet" << endl;
        }
        else{
            queueClients1=processing_function(eventNew, computersMass,clientNames,numTables,queueClients1);
        }
        eventTable=0;
        eventNew.table=0;
    }
    input1.close();
    sort(computersMass, computersMass + numTables, [](const Computer& a, const Computer& b) {
        return a.client < b.client;
    });
    for(int i=0;i<numTables;i++){
        if(computersMass[i].occupation==true){
            cout << setfill('0') << setw(2) << endTime.hours << ":" << setfill('0') << setw(2)<< endTime.minutes << " 11 " << computersMass[i].client <<endl;
            computersMass[i].occupation=false;
            computersMass[i].client="none";
            computersMass[i].allTimeUse=computersMass[i].allTimeUse + (endTime.hours*60 + endTime.minutes) - (computersMass[i].startTimeUse.hours*60 + computersMass[i].startTimeUse.minutes);
        }
    }
    cout << setfill('0') << setw(2) << endTime.hours << ":" << setfill('0') << setw(2)<< endTime.minutes << endl;
    Time itog;
    for(int i=0;i<numTables;i++){
        itog.hours = computersMass[i].allTimeUse/60;
        itog.minutes = computersMass[i].allTimeUse - (itog.hours * 60);
        cout << i+1 << " " << itog.hours * costPerHour + costPerHour<< " " << setfill('0') << setw(2) << itog.hours << ":" << setfill('0') << setw(2) << itog.minutes << endl;
    }
    return 0;
}
