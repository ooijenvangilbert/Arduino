

class darter
{
  private:
   int pos;
   
  public:   
    darter(int r, int g, int b)
    {
      pos = 0;
    }
    
    int update()
    {
      pos++;
      return pos;
    }

    int getposition()
    {
      return pos;
    }

    void setposition(int newValue)
    {
      pos = newValue;
    }
  
};
