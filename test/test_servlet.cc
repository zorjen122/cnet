#include <iostream>
#include <memory>
#include <string>
#include "httpServer.h"
#include "servlet.h"
#include "http.h"

class MyServlet : public Servlet
{
public:
    MyServlet() {}
    // ~MyServlet() {}
    MyServlet(const std::string &names) : name(names) {}
    int handle(HttpRequest::SharedPtr request,
               HttpResponse::SharedPtr response,
               HttpSession::SharedPtr session) override
    {
        std::cout << "Hello " << name << "!\n";
    }
    std::string name;
};

class MyServletCreator : public IServletCreator
{
public:
    MyServletCreator() {};
    ~MyServletCreator() {}
    Servlet::SharedPtr create() override
    {
        return Servlet::SharedPtr(new MyServlet("World"));
    };
};

int main()
{
    {
        Servlet::SharedPtr myserv;
        {
            ServletManager manager;
            // IServletCreator *creator = new MyServletCreator();
            MyServletCreator creator;
            myserv = creator.create();


            manager.addServlet("/name", myserv);
            auto ret = manager.getServlet("/name");
            std::cout << myserv.use_count() << "\n";
            
            HttpRequest::SharedPtr a(new HttpRequest);
            HttpResponse::SharedPtr b = std::make_shared<HttpResponse>();
            HttpSession::SharedPtr c(new HttpSession);

            ret->handle(a, b, c);
        }
        std::cout << myserv.use_count() << "\n";
    }
}