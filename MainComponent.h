#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
                    

{
public:
    //==============================================================================
    
    

    //==============================================================================
    MainComponent()
        :State(Stopped)

    {
        // Make sure you set the size of the component after
        // you add any child components.
        setSize(800, 600);

        // Some platforms require permissions to open input channels so request that here
        if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
            && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
        {
            juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
        }
        else
        {
            // Specify the number of input and output channels that we want to open
            setAudioChannels(2, 2);
        }


        addAndMakeVisible(&Open);
        Open.setButtonText("Open...");
        Open.onClick = [this] {Openfile(); };

        addAndMakeVisible(&Play);
        Play.setButtonText("Play");
        Play.onClick = [this] {PlayFile(); };
        Play.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

        addAndMakeVisible(&Stop);
        Stop.setButtonText("Stop");
        Stop.onClick = [this] {StopFile(); };
        Stop.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
        Stop.setEnabled(false);

        setSize(300, 200);

        FM.registerBasicFormats();
        //ATS.addChangeListener(this);
    }
    ~MainComponent() 
    {
        shutdownAudio();
    }
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    //==============================================================================
    // Your private member variables go here...
    enum ChangeState
    {
        Stopped,
        Starting,
        Playing,
        Stoping
    };
    void switchState(ChangeState newSate)
    {
        if (State != newSate)
        {
            State = newSate;
            switch (State)
            {
            case Stopped:
                Stop.setEnabled(false);
                Play.setEnabled(true);
                ATS.setPosition(0.0);
                break;
            case Starting:
                Play.setEnabled(false);
                ATS.start();
                break;
            case Playing:
                Stop.setEnabled(true);
                break;
            case Stoping:
                ATS.stop();
                break;
            default:
                break;
            }

        }
            
    }
    void Openfile()// Juce deals with a lot  data differently then normal 
        // and having a Abstaraction layer making the data cusom tpyes  
    {
        Choice = std::make_unique<juce::FileChooser>("Play some music !!!",
            juce::File{}, "*.wav", "*.mp3");

        // Custom data handling make using the auto keyword important here

        auto Flags = juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles;

        Choice->launchAsync(Flags, [this](const juce::FileChooser& fc)     
            {
                auto file = fc.getResult();

                if (file != juce::File{})                                                
                {
                    auto* reader = FM.createReaderFor(file);                

                    if (reader != nullptr)
                    {
                        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);   
                        ATS.setSource(newSource.get(), 0, nullptr, reader->sampleRate);       
                        Play.setEnabled(true);                                                     
                        Read.reset(newSource.release());                                          
                    }
                }
            });
    }

    void PlayFile()
    {
        switchState(Starting);
    }

    void StopFile()
    {
        switchState(Stoping);
    }
  
    // Button objects 
    juce::TextButton Open;
    juce::TextButton Play;
    juce::TextButton Stop;
    // File chooser INIT 
    std::unique_ptr<juce::FileChooser> Choice;
    // audio formats 
    juce::AudioFormatManager FM;
    // Audio Reader  
    juce::AudioTransportSource ATS;
    std::unique_ptr<juce::AudioFormatReaderSource> Read;
    ChangeState State;
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
