import dns.resolver

myResolver = dns.resolver.Resolver()
myResolver.nameservers = ['192.168.1.253']
query_object = myResolver.resolve(qname="www.test123.com", rdtype='A')  # 指定查询记录为A
print('查询对象：{}'.format(query_object))
 
print('*' * 100)
 
# 查看response对象
response_object = query_object.response
print('应答对象：{}'.format(response_object))
 
print('*' * 100)
 
# 查询answer对象
answer_object = response_object.answer
print('解析对象：{}'.format(answer_object))
 
print('*' * 100)
 
# 查看解析条目对象
answer_object = response_object.answer
for query_item in answer_object:
    print('查询条目：{}'.format(query_item))
 
print('*' * 100)
 
# 解析后的记录条目
query_item = response_object.answer[0]
# print(query_item)
for item in query_item:
    print('解析记录：{}'.format(item))
 
print('*' * 100)
