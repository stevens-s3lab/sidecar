-- payload.lua
wrk.method = "GET"
wrk.body   = string.rep("x", 1024) 
wrk.headers["Content-Type"] = "application/x-www-form-urlencoded"
--
