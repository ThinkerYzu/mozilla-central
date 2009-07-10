//
// Automatically generated by ipdlc.
// Edit at your own risk
//

#ifndef mozilla_plugins_NPPProtocolParent_h
#define mozilla_plugins_NPPProtocolParent_h

#include "mozilla/plugins/NPPProtocol.h"
#include "mozilla/ipc/RPCChannel.h"

namespace mozilla {
namespace plugins {


class /*NS_ABSTRACT_CLASS*/ NPPProtocolParent :
    public mozilla::ipc::RPCChannel::Listener
{
    friend class NPAPIProtocolParent;

protected:
    typedef mozilla::ipc::String String;
    typedef mozilla::ipc::StringArray StringArray;

    virtual nsresult AnswerNPN_GetValue(
                const String& key,
                String* value) = 0;

private:
    typedef IPC::Message Message;
    typedef mozilla::ipc::RPCChannel Channel;

public:
    NPPProtocolParent()
    {
    }

    virtual ~NPPProtocolParent()
    {
    }

    nsresult CallNPP_SetWindow(
                const NPWindow& window,
                NPError* rv)
    {
        Message __reply;
        Message* __msg;
        __msg = new NPPProtocol::Msg_NPP_SetWindow(window);
        __msg->set_routing_id(mPeerId);
        if (!(mChannel->Call(__msg, &(__reply)))) {
            return NS_ERROR_FAILURE;
        }
        if (!(NPPProtocol::Reply_NPP_SetWindow::Read(&(__reply), rv))) {
            return NS_ERROR_ILLEGAL_VALUE;
        }
        return NS_OK;
    }

    nsresult CallNPP_GetValue(
                const String& key,
                String* value)
    {
        Message __reply;
        Message* __msg;
        __msg = new NPPProtocol::Msg_NPP_GetValue(key);
        __msg->set_routing_id(mPeerId);
        if (!(mChannel->Call(__msg, &(__reply)))) {
            return NS_ERROR_FAILURE;
        }
        if (!(NPPProtocol::Reply_NPP_GetValue::Read(&(__reply), value))) {
            return NS_ERROR_ILLEGAL_VALUE;
        }
        return NS_OK;
    }

    virtual Result OnMessageReceived(const Message& msg)
    {
        switch (msg.type()) {
        default:
            {
                return MsgNotKnown;
            }
        }
    }

    virtual Result OnMessageReceived(
                const Message& msg,
                Message*& reply)
    {
        switch (msg.type()) {
        default:
            {
                return MsgNotKnown;
            }
        }
    }

    virtual Result OnCallReceived(
                const Message& msg,
                Message*& reply)
    {
        switch (msg.type()) {
        case NPPProtocol::Msg_NPN_GetValue__ID:
            {
                String key;
                String value;

                if (!(NPPProtocol::Msg_NPN_GetValue::Read(&(msg), &(key)))) {
                    return MsgPayloadError;
                }
                if (NS_FAILED(AnswerNPN_GetValue(key, &(value)))) {
                    return MsgValueError;
                }

                reply = new NPPProtocol::Reply_NPN_GetValue(value);
                reply->set_reply();
                return MsgProcessed;
            }
        default:
            {
                return MsgNotKnown;
            }
        }
    }

private:
    Channel* mChannel;
    int mId;
    int mPeerId;
    mozilla::ipc::IProtocolManager* mManager;
};


} // namespace plugins
} // namespace mozilla
#if 0

//-----------------------------------------------------------------------------
// Skeleton implementation of abstract actor class

// Header file contents
class ActorImpl :
    public NPPProtocolParent
{
    virtual nsresult AnswerNPN_GetValue(
                const String& key,
                String* value);
    ActorImpl();
    virtual ~ActorImpl();
};


// C++ file contents
nsresult ActorImpl::AnswerNPN_GetValue(
            const String& key,
            String* value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

ActorImpl::ActorImpl()
{
}

ActorImpl::~ActorImpl()
{
}

#endif // if 0

#endif // ifndef mozilla_plugins_NPPProtocolParent_h
