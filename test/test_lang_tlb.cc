#include <boost/test/auto_unit_test.hpp>

#include <test/testsuite.hh>
#include <utilmm/configfile/configset.hh>
#include <typelib/pluginmanager.hh>
#include <typelib/importer.hh>
#include <typelib/typemodel.hh>
#include <typelib/registry.hh>
#include <typelib/registryiterator.hh>
#include <typelib/typedisplay.hh>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace Typelib;
using namespace std;

BOOST_AUTO_TEST_CASE( test_tlb_rejects_recursive_types )
{
    PluginManager::self manager;
    Registry registry;

    static const char* test_file = TEST_DATA_PATH("test_cimport.h");
    utilmm::config_set config;
    config.set("include", TEST_DATA_PATH(".."));
    config.insert("define", "GOOD");
    manager->load("c", test_file, config, registry);

    BOOST_REQUIRE_THROW(manager->save("tlb", registry), ExportError);
}

BOOST_AUTO_TEST_CASE( test_tlb_idempotent )
{
    PluginManager::self manager;
    Registry registry;

    static const char* test_file = TEST_DATA_PATH("test_cimport.h");
    {
        utilmm::config_set config;
        config.set("include", TEST_DATA_PATH(".."));
        config.insert("define", "GOOD");
        config.insert("define", "NO_RECURSIVE_TYPE");
        manager->load("c", test_file, config, registry);
    }

    string result;
    BOOST_REQUIRE_NO_THROW(result = manager->save("tlb", registry));
    istringstream io(result);
    utilmm::config_set config;
    Registry* reloaded;
    BOOST_REQUIRE_NO_THROW(reloaded = manager->load("tlb", io, config));

    if (!registry.isSame(*reloaded))
    {
        RegistryIterator it = registry.begin(),
                         end = registry.end();
        for (; it != end; ++it)
        {
            if (!reloaded->has(it->getName(), true))
                std::cerr << "reloaded does not have " << it->getName() << std::endl;
            else if (!(*it).isSame(*reloaded->build(it->getName())))
            {
                std::cerr << "versions of " << it->getName() << " differ" << std::endl;
                std::cerr << *it << std::endl << std::endl;
                std::cerr << *reloaded->get(it->getName()) << std::endl;
            }
        }
    }
}

BOOST_AUTO_TEST_CASE( test_tlb_import )
{
    PluginManager::self manager;
    Importer* importer = manager->importer("tlb");
    utilmm::config_set config;

    {
	string empty_tlb = "<?xml version=\"1.0\"?>\n<typelib>\n</typelib>";
	istringstream stream(empty_tlb);
	Registry registry;
	importer->load(stream, config, registry);
    }

    { 
	ifstream file(TEST_DATA_PATH("rflex.tlb"));
	Registry registry;
	importer->load(file, config, registry);

	BOOST_CHECK( registry.get("/custom_null") );
	BOOST_CHECK( registry.get("/custom_null")->isNull() );
    }
}

