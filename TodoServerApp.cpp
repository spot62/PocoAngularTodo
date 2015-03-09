#include <iostream>
#include <string>

#include "TodoServerApp.h"

Mutex TodoServerApp::todoLock;
CTodoList TodoServerApp::todoList;

ostream& operator<<(ostream& os, CTodo& todo)
{
    os << "{ \"_id\": "<< todo.getId() <<  ", \"text\": \"" << todo.getText() << "\" }";
    return os;
}

ostream& operator<<(ostream& os, CTodoList& todoList)
{
    map<size_t, CTodo> todos = todoList.readList();

    os << "[";
    if(!todos.empty())
    {
        if(todos.size() == 1)
            os << todos.begin()->second;
        else
            for ( map<size_t, CTodo>::iterator it = todos.begin();;)
            {
                os << it->second ;
                if(++it != todos.end())
                    os << ',';
                else
                    break;
            }

    }
    os << "]\n";

    return os;
}

class CTodoHandler : public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        URI uri(req.getURI());
        string method = req.getMethod();

        cerr << "URI: " << uri.toString() << endl;
        cerr << "Method: " << req.getMethod() << endl;

        StringTokenizer tokenizer(uri.getPath(), "/", StringTokenizer::TOK_TRIM);
        HTMLForm form(req,req.stream());

        if(!method.compare("POST"))
        {
            cerr << "Create:" << form.get("text") << endl;
            CTodo todo(form.get("text"));
            TodoServerApp::createTodo(todo);
        }
        else if(!method.compare("PUT"))
        {
            cerr << "Update id:" << *(--tokenizer.end()) << endl;
            cerr << "Update text:" << form.get("text") << endl;
            //size_t id=stoull(*(--tokenizer.end()));
            //TodoServerApp::updateTodo(id, form.get("text"));
        }
        else if(!method.compare("DELETE"))
        {
            cerr << "Delete id:" << *(--tokenizer.end()) << endl;
            size_t id=stoull(*(--tokenizer.end()));
            TodoServerApp::deleteTodo(id);
        }

        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("application/json");
        //resp.setContentLength(...);
        ostream& out = resp.send();

        cerr << TodoServerApp::readTodoList() << endl;
        out << TodoServerApp::readTodoList() << endl;

        out.flush();
    }
};

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <map>          // std::ifstream

class CFileHandler : public HTTPRequestHandler
{
    typedef std::map<const std::string, const std::string> TStrStrMap;
    TStrStrMap CONTENT_TYPE = {
#include "MimeTypes.h"
    };

    string getPath(string& path){

        if(path == "/"){
            path="/index.html";
        }

        path.insert(0, "./www");

        return path;
    }

    string getContentType(string& path){

        string contentType("text/plain");
        Poco::Path p(path);

        TStrStrMap::const_iterator i=CONTENT_TYPE.find(p.getExtension());

        if (i != CONTENT_TYPE.end())
        { /* Found, i->first is f, i->second is ++-- */
           contentType = i->second;
        }

        if(contentType.find("text/") != std::string::npos)
        {
            contentType+="; charset=utf-8";
        }

        cerr << path << " : " << contentType << endl;

        return contentType;
    }

public:

    void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        cerr << "Get static page: ";
        //system("echo -n '1. Current Directory is '; pwd");

        URI uri(req.getURI());
        string path(uri.getPath());

        ifstream ifs (getPath(path).c_str(), ifstream::in);

        if(ifs)
        {
            resp.setStatus(HTTPResponse::HTTP_OK);
            resp.setContentType(getContentType(path));
            ostream& out = resp.send();

            char c = ifs.get();

            while (ifs.good()) {
                out << c;
                c = ifs.get();
            }

            out.flush();
        }
        else
        {
            resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            ostream& out = resp.send();

            out << "File not found" << endl;

            out.flush();
        }

        ifs.close();
    }
};

class TodoRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
    virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest & request)
    {
        if (!request.getURI().find("/api/"))
            return new CTodoHandler;
        else
            return new CFileHandler;
    }
};

void TodoServerApp::createTodo(CTodo& todo)
{
    ScopedLock<Mutex> lock(todoLock);
    todoList.create(todo);
}

CTodoList& TodoServerApp::readTodoList()
{
    ScopedLock<Mutex> lock(todoLock);
    return todoList;
}

void TodoServerApp::deleteTodo(size_t id)
{
    ScopedLock<Mutex> lock(todoLock);
    todoList.del(id);
}

int TodoServerApp::main(const vector<string> &)
{
    HTTPServerParams* pParams = new HTTPServerParams;

    pParams->setMaxQueued(100);
    pParams->setMaxThreads(16);

    HTTPServer s(new TodoRequestHandlerFactory, ServerSocket(8000), pParams);

    s.start();
    cerr << "Server started" << endl;

    waitForTerminationRequest();  // wait for CTRL-C or kill

    cerr << "Shutting down..." << endl;
    s.stop();

    return Application::EXIT_OK;
}


