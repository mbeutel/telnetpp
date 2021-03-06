#include "telnetpp/options/mccp/zlib/decompressor.hpp"
#include <boost/optional.hpp>
#include <zlib.h>
#include <cassert>

namespace telnetpp { namespace options { namespace mccp { namespace zlib {

namespace {

// In a Telnet application, we don't expect to be receiving massive amounts
// of data at a time; usually only a few bytes.  Therefore, it's not necessary
// to allocate massive blocks of 128KB or 256KB as suggested by the ZLib
// how-to guide.  Instead, we can just use small blocks on the stack and
// iterate in the very rare case that a single message yields a block of 1KB
// or more.
static constexpr std::size_t receive_buffer_size = 1024;

}

// ==========================================================================
// DECOMPRESSOR::IMPL
// ==========================================================================
struct decompressor::impl
{
    boost::optional<z_stream> stream;

    // ======================================================================
    // CONSTRUCT_STREAM
    // ======================================================================
    void construct_stream()
    {
        assert(stream == boost::none);

        stream = z_stream{};

        auto result = inflateInit(stream.get_ptr());
        assert(result == Z_OK);
    }

    // ======================================================================
    // DESTROY_STREAM
    // ======================================================================
    void destroy_stream()
    {
        assert(stream != boost::none);

        auto result = inflateEnd(stream.get_ptr());
        assert(result == Z_OK || result == Z_STREAM_ERROR);

        stream = boost::none;
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
decompressor::decompressor()
  : pimpl_(new impl)
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
decompressor::~decompressor()
{
    if(pimpl_->stream)
    {
        pimpl_->destroy_stream();
    }
}

// ==========================================================================
// DO_START
// ==========================================================================
void decompressor::do_start()
{
}

// ==========================================================================
// DO_FINISH
// ==========================================================================
void decompressor::do_finish(continuation const &cont)
{
    if(pimpl_->stream)
    {
        pimpl_->destroy_stream();
    }
}

// ==========================================================================
// TRANSFORM_CHUNK
// ==========================================================================
telnetpp::bytes decompressor::transform_chunk(
    telnetpp::bytes data,
    continuation const &cont)
{
    if (!pimpl_->stream)
    {
        pimpl_->construct_stream();
    }

    byte receive_buffer[receive_buffer_size];

    pimpl_->stream->avail_in  = data.size();
    pimpl_->stream->next_in   = const_cast<telnetpp::byte *>(data.data());
    pimpl_->stream->avail_out = receive_buffer_size;
    pimpl_->stream->next_out  = receive_buffer;

    auto response = inflate(pimpl_->stream.get_ptr(), Z_SYNC_FLUSH);

    if (response == Z_DATA_ERROR)
    {
        throw corrupted_stream_error(
            "Inflation of byte in ZLib stream yielded Z_DATA_ERROR");
    }

    assert(response == Z_OK || response == Z_STREAM_END);

    auto const received_data = telnetpp::bytes{
        receive_buffer, pimpl_->stream->next_out
    };

    bool const stream_ended = response == Z_STREAM_END;
    data = data.subspan(data.size() - pimpl_->stream->avail_in);

    if (stream_ended)
    {
        pimpl_->destroy_stream();
    }

    cont(received_data, stream_ended);

    return data;
}

}}}}
