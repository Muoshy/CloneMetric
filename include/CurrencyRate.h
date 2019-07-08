#pragma once
#include <JsonListener.h>
#include <JsonStreamingParser.h>

typedef struct CurrencyRateData {
  int USD;
  int EUR;
  int usdF;
  int eurF;


} CurrencyRateData;

class CurrencyRate: public JsonListener {
  private:
    String currentKey;
    String currentParent;
    CurrencyRateData *data;
    void doUpdate(CurrencyRateData *data, String url);

  public:
    CurrencyRate();
    
    void updateCurrent(CurrencyRateData *data, String url);
    
    virtual void whitespace(char c);

    virtual void startDocument();

    virtual void key(String key);

    virtual void value(String value);

    virtual void endArray();

    virtual void endObject();

    virtual void endDocument();

    virtual void startArray();

    virtual void startObject();
};