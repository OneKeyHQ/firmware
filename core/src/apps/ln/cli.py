 #-*- coding: UTF-8 -*- 
from concurrent import futures
import time
import grpc
import remotesigner_pb2
import remotesigner_pb2_grpc
 

def run():
    # 连接 rpc 服务器
    channel = grpc.insecure_channel('localhost:50051')
    # 调用 rpc 服务
    stub = remotesigner_pb2_grpc.VersionStub(channel)
    response = stub.Version(remotesigner_pb2.VersionRequest())
    print("hsmd version: " + response.version_string)
    #response = stub.Ping(remotesigner_pb2.PingRequest(message="Hello"))

    stub = remotesigner_pb2_grpc.SignerStub(channel)
    response = stub.Ping(remotesigner_pb2.PingRequest())
    print("hsmd ping reply: %s" % response)

    req = remotesigner_pb2.InitRequest()
    response = stub.Init(req)
    print("hsmd init: %s" % response)

if __name__ == '__main__':
    run()
