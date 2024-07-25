-- ind1 = _G["ind1"]
-- ind2 = _G["ind2"]
-- res = _G["res"]

for i = 2, #ind1 do
  res[i] = res[i-1] + i
end

-- test_tbl["arr"] = {1,2,3,4}
-- print('This snippet is used to demonstrate low level Lua calls from '..test_str ..' '..test_num .. ' ' ..tostring(test_bool))
-- test_bool=false
-- test_str='hi C++ from Lua'
-- print('The value of test_tbl[t2] = '..test_tbl["t2"])
-- test_tbl["t3"] = { name = 'lua'}
-- test_num=4.3
