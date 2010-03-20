module daemon.router;

private import tango.time.Time;

private import daemon.client;
private import daemon.friend;
private import daemon.server;
private import lib.asset;
private import lib.message;

private class ForwardedAsset : IServerAsset {
private:
    Router router;
    daemon.client.OpenRequest req;
    IAsset[] backingAssets;
    BHServerOpenCallback openCallback;
package:
    uint waitingResponses;
public:
    this (Router router, daemon.client.OpenRequest req, BHServerOpenCallback cb)
    {
        this.router = router;
        this.req = req;
        this.openCallback = cb;
    }
    ~this() {
        close();
    }

    void close() {
        assert(waitingResponses == 0); // We've got to fix timeouts some day.
        foreach (asset; backingAssets)
            asset.close();
    }

    void aSyncRead(ulong offset, uint length, BHReadCallback cb) {
        backingAssets[0].aSyncRead(offset, length, cb);
    }
    ulong size() {
        return backingAssets[0].size;
    }
    void addBackingAsset(IAsset asset, Status status, lib.message.OpenOrUploadRequest req, OpenResponse resp) {
        switch (status) {
        case Status.SUCCESS:
            assert(asset, "SUCCESS response, but no asset");
            backingAssets ~= asset;
            break;
        default:
            break;
        }
        waitingResponses -= 1;
        if (waitingResponses <= 0)
            doCallback();
    }
    mixin IRefCounted.Impl;
package:
    void doCallback() {
        router.openRequests.remove(req.uuid);
        openCallback(this, (backingAssets.length > 0) ? Status.SUCCESS : Status.NOTFOUND);
    }
}

class Router : IAssetSource {
private:
    Server server;
    ForwardedAsset[ulong] openRequests;
    Friend[Client] connectedFriends;
public:
    this(Server server) {
        this.server = server;
    }

    ForwardedAsset findAsset(daemon.client.OpenRequest req, BHServerOpenCallback cb) {
        if (req.uuid in openRequests)
            req.callback(null, Status.WOULD_LOOP);
        else
            return forwardOpenRequest(req, cb);
    }

    void registerFriend(Friend f) {
        connectedFriends[f.c] = f;
    }

    Friend unregisterFriend(Client c) {
        if (c in connectedFriends) {
            auto friend = connectedFriends[c];
            connectedFriends.remove(c);
            return friend;
        } else {
            return null;
        }
    }
private:
    ForwardedAsset forwardOpenRequest(daemon.client.OpenRequest req, BHServerOpenCallback cb) {
        bool forwarded = false;
        auto asset = new ForwardedAsset(this, req, cb);
        asset.takeRef();
        foreach (friend; connectedFriends) {
            auto client = friend.c;
            if (client != req.client) {
                asset.waitingResponses += 1;
                // TODO: Randomize timeouts
                client.open(req.ids, &asset.addBackingAsset, req.uuid, TimeSpan.fromMillis(req.timeout-50));
                forwarded = true;
            }
        }
        if (!forwarded) {
            asset.doCallback();
            delete asset;
            return null;
        } else {
            openRequests[req.uuid] = asset;
            return asset;
        }
    }
}