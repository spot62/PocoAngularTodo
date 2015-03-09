#pragma once

#include <Poco/Mutex.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Path.h>
#include <Poco/ScopedLock.h>
#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>
#include <Poco/Util/ServerApplication.h>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;

/**
    Todo
*/
class CTodo {
    size_t id;
    string text;
public:
    CTodo(string text): text(text){
    }
    /* getters & setters */
    size_t getId(){ return id; }
    void setId(size_t id){ this->id = id; }
    string getText(){ return text; }
};

/**
    Список Todo
*/
class CTodoList {
    size_t id;
    map<size_t, CTodo> todos;

public:
    CTodoList():id(0){}
    /* CRUD */
    void create(CTodo& todo){ todo.setId(++id); todos.insert(pair<size_t,CTodo>(id, todo)); };
    map<size_t, CTodo>& readList(){ return todos; }
    void del(size_t id){ todos.erase(id); };

};

/**
    Сервер
*/
class TodoServerApp : public ServerApplication
{
public:
    /* CRUD */
    static void createTodo(CTodo& todo);
    static CTodoList& readTodoList();
    //static void updateTodo(size_t id, CTodo& todo);
    static void deleteTodo(size_t id);

protected:
    int main(const vector<string> &);
    static Mutex todoLock;
    static CTodoList todoList;
};
