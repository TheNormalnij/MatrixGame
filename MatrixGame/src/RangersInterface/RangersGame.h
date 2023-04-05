
class RangersGame {
public:
    RangersGame() = default;
    ~RangersGame() = default;

    void SetProgressBar(float val);

    void Begin();
    void DoTact();

    float GetMusicVolume();
    void SetMusicVolume(float volume);
};