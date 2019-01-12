
#include "../include/compressed_streams/istream_view.h"

namespace compressed_streams
{

class IStreamBufView: public std::streambuf
{
public:
    IStreamBufView(const char* data, size_t len)
    {
        char* data_start = const_cast<char*>(data);
        setg(data_start, data_start, data_start + len);
    }

    virtual ~IStreamBufView()
    {}

    virtual std::streamsize showmanyc()
    {
        return std::distance(gptr(), epptr());
    }
};


IStreamView::IStreamView(const char *data, const size_t size):
    std::istream(new IStreamBufView(data, size))
{}


IStreamView::~IStreamView()
{
    delete rdbuf();
}



}   // compressed streams
