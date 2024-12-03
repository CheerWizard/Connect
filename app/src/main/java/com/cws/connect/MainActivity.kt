package com.cws.connect

import android.app.NativeActivity
import android.content.Context
import android.content.pm.PackageManager
import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioRecord
import android.os.Bundle
import android.util.Log

class MainActivity : NativeActivity() {

    companion object {
        private const val RC_RECORD_AUDIO = 1
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestPermissions()
        Log.d(MainActivity::class.simpleName, "AudioConfig signature = ${JniUtil.getJNIClassSignature(AudioConfig::class.java)}")
        Log.d(MainActivity::class.simpleName, "supportRecording signature = ${JniUtil.getJNIClassSignature(Boolean::class.java)}")
        Log.d(MainActivity::class.simpleName, "sampleRate signature = ${JniUtil.getJNIClassSignature(Int::class.java)}")
        Log.d(MainActivity::class.simpleName, "sampleBufferSize signature = ${JniUtil.getJNIClassSignature(Int::class.java)}")
        Log.d(MainActivity::class.simpleName, "delayInMillis signature = ${JniUtil.getJNIClassSignature(Long::class.java)}")
        Log.d(MainActivity::class.simpleName, "decay signature = ${JniUtil.getJNIClassSignature(Float::class.java)}")
    }

    private fun requestPermissions() {
        if (checkSelfPermission(android.Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(
                arrayOf(android.Manifest.permission.RECORD_AUDIO),
                RC_RECORD_AUDIO
            )
            return
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            RC_RECORD_AUDIO -> {
                if (grantResults.size != 1 ||
                    grantResults[0] != PackageManager.PERMISSION_GRANTED
                ) {
                    // permission denied for audio recording
                    // for now will just finish activity and remove from task manager
                    finishAndRemoveTask()
                    return
                }
            }
        }
    }

    fun getAudioConfig(): AudioConfig {
        val audioManager = getSystemService(Context.AUDIO_SERVICE) as AudioManager?

        var supportRecording = false
        var sampleRate = 0
        var sampleBufferSize = 0

        audioManager?.let {
            supportRecording = true
            sampleRate = it.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)?.toInt() ?: 0
            sampleBufferSize = it.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)?.toInt() ?: 0
        }

        val recordBufferSize = AudioRecord.getMinBufferSize(
            sampleRate,
            AudioFormat.CHANNEL_IN_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        )

        if (recordBufferSize == AudioRecord.ERROR || recordBufferSize == AudioRecord.ERROR_BAD_VALUE) {
            supportRecording = false
        }

        return AudioConfig(
            supportRecording = supportRecording,
            sampleRate = sampleRate,
            sampleBufferSize = sampleBufferSize,
            delayInMillis = 0,
            decay = 0.0f
        )
    }

}