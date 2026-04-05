const express = require("express");
const mongoose = require("mongoose");
const bcrypt = require("bcryptjs");
const jwt = require("jsonwebtoken");
const cors = require("cors");

const app = express();
app.use(express.json());
app.use(cors());

mongoose.connect("mongodb://127.0.0.1:27017/energyDB");

// USER
const User = mongoose.model("User",{
name:String,email:String,password:String
});

// DATA
let energyData = { voltage:0,current:0,power:0 };

// AUTH
app.post("/signup", async(req,res)=>{
const hashed=await bcrypt.hash(req.body.password,10);
await new User({...req.body,password:hashed}).save();
res.send("User Created");
});

app.post("/login", async(req,res)=>{
const user=await User.findOne({email:req.body.email});
if(!user)return res.send("No user");

const ok=await bcrypt.compare(req.body.password,user.password);
if(!ok)return res.send("Wrong");

const token=jwt.sign({id:user._id},"SECRET");
res.json({token});
});

// DATA API
app.post("/update",(req,res)=>{
energyData=req.body;
res.send("OK");
});

app.get("/data",(req,res)=>{
res.json(energyData);
});

app.listen(3000,()=>console.log("Server running"));