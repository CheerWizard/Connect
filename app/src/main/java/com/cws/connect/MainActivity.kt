package com.cws.connect

import android.app.NativeActivity
import android.os.Bundle
import com.cws.connect.databinding.ActivityMainBinding

class MainActivity : NativeActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

}