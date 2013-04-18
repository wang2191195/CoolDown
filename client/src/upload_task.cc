#include "upload_task.h"
#include <Poco/SharedMemory.h>
#include <Poco/Format.h>
#include <Poco/Exception.h>

using Poco::SharedMemory;
using Poco::format;
using Poco::Exception;

namespace CoolDown{
    namespace Client{

        UploadTask::UploadTask(const FilePtr& file, UInt64 offset, int chunk_size, const SockPtr& sock)
        :Task( format("Upload task to %s", sock->peerAddress().host().toString()) ), 
        file_(file),
        offset_(offset),
        chunk_size_(chunk_size),
        sock_(sock)
        {
            
        }
        UploadTask::~UploadTask(){

        }

        void UploadTask::runTask(){
            SharedMemory sm(*file_, SharedMemory::AM_READ);
            int nSend = sock_->sendBytes( sm.begin() + offset_, chunk_size_ );
            if( nSend != chunk_size_ ){
                throw Exception( format("chunk_size is %d bytes but only send %d", chunk_size_, nSend) );
            }
        }
    }
}