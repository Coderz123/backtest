ind1 = {0,0,1,1}
ind2 = {0,1,0,1}
res = {}

for i = 1, #ind1 do
  if ind1[i] == 0 and ind2[i] == 0 then res[i] = 0
  elseif ind1[i] == 0 and ind2[i] == 1 then res[i] = 1
  elseif ind1[i] == 1 and ind2[i] == 0 then res[i] = 1
  elseif ind1[i] == 1 and ind2[i] == 1 then res[i] = 0
  end
end

print (res[1] .. res[2] .. res[3] .. res[4])
-- test_tbl["arr"] = {1,2,3,4}
-- print('This snippet is used to demonstrate low level Lua calls from '..test_str ..' '..test_num .. ' ' ..tostring(test_bool))
-- test_bool=false
-- test_str='hi C++ from Lua'
-- print('The value of test_tbl[t2] = '..test_tbl["t2"])
-- test_tbl["t3"] = { name = 'lua'}
-- test_num=4.3
