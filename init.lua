if (srv ~= nil)then 
	srv:close()
end
wifi.setmode(wifi.SOFTAP);
wifi.ap.config({ssid="Lichterkette",pwd="12345678"});
srv=net.createServer(net.TCP,20) 
srv:listen(80,function(conn) 

	local rnrn=0
	local Status = 0
	local DataToGet = 0
	local method=""
	local url=""
	local vars=""
	
	conn:on("receive",function(conn,payload)
		_, _, method, url, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")
		local _GET={}
		if (vars ~= nil)then 
			for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do 
				_GET[k] = v
				print(k,v)
			end

		end
		local _color,_effekt,_len, _speed, _count = 0,0,0,0,150
        if(_GET.color == "RED")then
              _color = 128;
        elseif(_GET.color == "GREEN")then
              _color = 16;
		elseif(_GET.color == "BLUE")then
              _color = 2;
		elseif(_GET.color == "TURQUOIS")then
              _color = 25;
		elseif(_GET.color == "ORANGE")then
              _color = 164;
		elseif(_GET.color == "CYAN")then
              _color = 18;
		elseif(_GET.color == "WHITE")then
              _color = 255;
		elseif(_GET.color == "VIOLETT")then
              _color = 131;
		elseif(_GET.color == "PINK")then
              _color = 129;
		elseif(_GET.color == "YELLOW")then
              _color = 176;
		elseif(_GET.color == "BLACK")then
              _color = 0;
        end
		if(_GET.effect == "SETFULLCOLOR")then
              _effect = 128;
			  softuart.write(1,38400,254, 6, _effect ,1, _count, _color);
        elseif(_GET.effect == "FILLCOLOR")then
              _effect = 129;
			  softuart.write(1,38400,254, 7, _effect ,25, _count, _color,0);
		elseif(_GET.effect == "BLINK")then
			  _effect = 130;
              softuart.write(1,38400,254, 6, _effect ,20, _count, _color);
		elseif(_GET.effect == "RUNLED")then
			  _effect = 131;
              softuart.write(1,38400,254, 7, _effect ,55, _count, _color,0);	  
		elseif(_GET.effect == "ALTERNATING")then
			  _effect = 133;
              softuart.write(1,38400,254, 7, _effect ,75, _count, _color,0);
		elseif(_GET.effect == "RECOLOR")then
			  _effect = 135;
              softuart.write(1,38400,254, 6, _effect ,50, _count, _color);
		elseif(_GET.effect == "FADE")then
			  _effect = 136;
              softuart.write(1,38400,254, 6, _effect ,25, _count, _color);
		elseif(_GET.effect == "RAINBOW")then
			  _effect = 137;
              softuart.write(1,38400,254, 5, _effect ,150, _count);
        end
		
		DataToGet = -1
		if url == "favicon.ico" then
			conn:send("HTTP/1.1 404 file not found")
			return
		end
		if url == "" then
			conn:send("HTTP/1.1 200 OK\r\n")
			conn:send("Content-Type: text/html\r\n")
			conn:send("Content-Length: 3045\r\n")
			conn:send("Connection: keep-alive\r\n")
			conn:send("\r\n")
			print("Index OK")
			url="index.html"
			DataToGet = 0
		elseif url=="strings.js" then
			conn:send("HTTP/1.1 200 OK\r\n")
			conn:send("Content-Type: application/javascript\r\n")
			conn:send("Content-Length: 55\r\n")
			conn:send("Connection: keep-alive\r\n")
			conn:send("\r\n")
			return
		elseif url~="" then
			conn:send("HTTP/1.1 200 OK\r\n")
			conn:send("Connection: keep-alive\r\n")
			conn:send("\r\n")
			DataToGet = 0
			return
		else
			conn:send("HTTP/1.1 200 OK\r\n\r\n")
			print("OK")
		end    

	end)

	conn:on("sent",function(conn)
		if DataToGet>=0 and method=="GET" then
			if(url ~= "strings.js") then
				if file.open(url, "r") then
					file.seek("set", DataToGet)
					local line=file.read(512)
					file.close()
					if line then
						conn:send(line)
						DataToGet = DataToGet + 512
						if (string.len(line)==512) then
							line = nil
							return
						end
					end
				end
			else
				conn:close()
				return
			end
		end
    conn:close() 
  end)

end)