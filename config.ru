require "sinatra"

get "/ping" do
   "@pong!"
end

post "/hola" do
  par = JSON.parse request.body.read
  puts "POSITION: #{par["x"]}:#{par["y"]}"
  "@up"
end

get "/hola" do
  "@up"
end

run Sinatra::Application
