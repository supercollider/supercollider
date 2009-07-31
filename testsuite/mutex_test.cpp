#include <boost/test/unit_test.hpp>

#include "../source/utilities/mutex.hpp"
#include <boost/shared_ptr.hpp>

struct foo2:
    virtual public nova::lockable
{
    friend class nova::scoped_lock;
};

#include <vector>
                 
BOOST_AUTO_TEST_CASE( mutex_test )
{
    using namespace nova;
    Mutex mut;

    mut.lock();
    
    BOOST_REQUIRE_EQUAL( mut.try_lock(), true );
    {
        scoped_lock lock(mut);
    }       
    mut.unlock();
    mut.unlock();
    {
        scoped_lock lock(mut);
    }       
    
    {
        foo2 f;
        scoped_lock lock(f);
    
        const foo2 g;
        scoped_lock lock2(g);
    }       
    
    std::vector<boost::shared_ptr<foo2> > fv;
    
    BOOST_REQUIRE_EQUAL( (trylock_all2<std::vector<boost::shared_ptr<foo2> >, foo2, const lockable, &lockable::try_lock, &lockable::unlock > (fv)) , true );
}
