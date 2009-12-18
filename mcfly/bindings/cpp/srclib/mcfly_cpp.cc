#include <mcfly_cpp.hh>


Mcfly::Mcfly(const char *config)
{
    mcfly_init(config, &mMcfly);
}


Mcfly::~Mcfly()
{
    mcfly_shutdown(mMcfly);
}
