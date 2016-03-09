#pragma once

class StdinAudioListener : public IAudioListener
{
	public:
		int RecordAudioStream(IAudioSink*) override;
		StdinAudioListener();
}
