/*
 * InAppPurchaseAndroid.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: kristian.mitra
 */
#include "InAppPurchase.h"

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

static InAppPurchase* s_sharedInAppPurchase;

ProductObject::~ProductObject()
{
    if (localizedDescription) {
        localizedDescription->release();
        localizedDescription = NULL;
    }

    if (localizedTitle) {
        localizedTitle->release();
        localizedTitle = NULL;
    }

    if (productIdentifier) {
        productIdentifier->release();
        productIdentifier = NULL;
    }

    if (priceString) {
        priceString->release();
        priceString = NULL;
    }

    if (currencyString) {
        currencyString->release();
        currencyString = NULL;
    }
}

InAppPurchase* InAppPurchase::sharedInAppPurchase()
{
    if (s_sharedInAppPurchase == NULL)
    {
        s_sharedInAppPurchase = new InAppPurchase();
        s_sharedInAppPurchase->init();
    }
    return s_sharedInAppPurchase;
}


bool InAppPurchase::init()
{
    m_delegate = NULL;
    m_product = NULL;

    return true;
}

void InAppPurchase::getProduct()
{
    if (m_product) {
        if (m_delegate) {
            m_delegate->getProductComplete(m_product);
        }
        return;
    }

	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"ppl/cocos2dx/ranchrun/iab/InAppBillingHelper",    	// fully qualified class name
			"querySKUDetails",                              	// method name
			"()V")) {             		   						// data type of argument

		// call the method, with arguments
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}


void InAppPurchase::buyProductWithIdentifier(const char *identifier)
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"ppl/cocos2dx/ranchrun/iab/InAppBillingHelper",    	// fully qualified class name
			"buySKU",                              				// method name
			"(Ljava/lang/String;)V")) {             		   	// data type of argument

		jstring stringProductID = methodInfo.env->NewStringUTF(identifier);

		// call the method, with arguments
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringProductID);

		methodInfo.env->DeleteLocalRef(stringProductID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}


// Helper functions for splitting strings to parse results ---------------

std::vector<std::string> &mainSplit(const std::string &s, char delim, std::vector<std::string> &elems) {

	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

// Splits the string with a defined delimiter
// Returns a vector containing the separated string elements
std::vector<std::string> InAppPurchase::customSplit(const std::string &s, char delim) {
    std::vector<std::string> elems;
    mainSplit(s, delim, elems);
    return elems;
}



void InAppPurchase::onAndroidQuerySKUDetailsCallback(std::string resultString) {

	int IDX_PRODUCT_ID 		= 0;
	int IDX_DESCRIPTION 	= 1;
	int IDX_TITLE			= 2;
	int IDX_PRICE_STRING	= 3;
	int IDX_CURRENCY_STRING = 4;
	//int IDX_GEM_AMOUNT		= 5;

	if(resultString.empty()) {
		if (m_delegate) {
			m_delegate->getProductComplete(NULL);
		}
		return;
	}

	char productDelimiter = '|';
	char infoDelimiter = '-';

	// Parse each product info
	std::vector<std::string> vProducts = InAppPurchase::customSplit(resultString, productDelimiter);
	const int size = vProducts.size();

	if (size > 0) {

		CCArray *array = CCArray::createWithCapacity(size);
		CCLog("array size: %d", size);

		for(auto &productItem : vProducts) {

			CCLog("QQQ product: %s", productItem.c_str());

			// Parse each info of the product
			std::vector<std::string> vInfo = InAppPurchase::customSplit(productItem, infoDelimiter);
			CCLog("QQQ idx1: %s", vInfo[IDX_PRODUCT_ID].c_str());
			CCLog("QQQ idx2: %s", vInfo[IDX_DESCRIPTION].c_str());
			CCLog("QQQ idx3: %s", vInfo[IDX_TITLE].c_str());
			CCLog("QQQ idx4: %s", vInfo[IDX_PRICE_STRING].c_str());
			CCLog("QQQ idx5: %s", vInfo[IDX_CURRENCY_STRING].c_str());

			ProductObject *object = new ProductObject;

			object->productIdentifier = CCString::create(vInfo[IDX_PRODUCT_ID].c_str());
			object->productIdentifier->retain();

			if(vInfo[IDX_DESCRIPTION].empty()) {
				object->localizedDescription = CCString::create("");
			}
			else {
				object->localizedDescription = CCString::create(vInfo[IDX_DESCRIPTION].c_str());
			}
			object->localizedDescription->retain();

			if(vInfo[IDX_TITLE].empty()) {
				object->localizedTitle = CCString::create("");
			}
			else {
				object->localizedTitle = CCString::create(vInfo[IDX_TITLE].c_str());
			}
			object->localizedTitle->retain();

			// price string
			object->priceString = CCString::create(vInfo[IDX_PRICE_STRING].c_str());
			object->priceString->retain();

			// currency string
			object->currencyString = CCString::create(vInfo[IDX_CURRENCY_STRING].c_str());
			object->currencyString->retain();

			// price (float)
			object->price = ::atof(vInfo[IDX_PRICE_STRING].c_str());

			// gem value (int)
			//object->gemAmount = ::atoi(vInfo[IDX_GEM_AMOUNT].c_str());

			array->addObject(object);
			object->autorelease();
		}

		m_product = array;
		m_product->retain();

		if (m_delegate) {
			m_delegate->getProductComplete(m_product);
		}
	}
	else {
		if (m_delegate) {
			m_delegate->getProductComplete(NULL);
		}
	}
}

void InAppPurchase::onAndroidPurchaseDidCompleteTransaction(std::string transactionIdentifier, std::string transactionReceipt) {

	if(m_delegate) {
		m_delegate->inAppPurchaseDidCompleteTransaction(this, transactionIdentifier.c_str(),  transactionReceipt.c_str());
	}
}

void InAppPurchase::onAndroidPurchaseDidFailedTransaction(std::string transactionIdentifier, std::string transactionReceipt) {

	if(m_delegate) {
		m_delegate->inAppPurchaseDidFailedTransaction(this, transactionIdentifier.c_str(),  transactionReceipt.c_str());
	}
}

void InAppPurchase::onAndroidPurchaseDidRestoreTransaction(std::string transactionIdentifier, std::string transactionReceipt) {

	if(m_delegate) {
		m_delegate->inAppPurchaseDidRestoreTransaction(this, transactionIdentifier.c_str(),  transactionReceipt.c_str());
	}
}




JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onQuerySKUDetailsCallback(JNIEnv * env, jobject  obj, jstring resultString)
{
	const char *tmpResultString = env->GetStringUTFChars(resultString, 0);
	std::string queryResultString = (tmpResultString);

	InAppPurchase::sharedInAppPurchase()->onAndroidQuerySKUDetailsCallback(queryResultString);

	env->ReleaseStringUTFChars(resultString, tmpResultString);
}


JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onPurchaseCompleteCallback(JNIEnv * env, jobject  obj, jstring transactionID, jstring transactionReceipt)
{
	const char *tmpTransactionIDString = env->GetStringUTFChars(transactionID, 0);
	const char *tmpTransactionReceiptString = env->GetStringUTFChars(transactionReceipt, 0);

	std::string transactionIDString = (tmpTransactionIDString);
	std::string transactionReceiptString = (tmpTransactionReceiptString);

	InAppPurchase::sharedInAppPurchase()->onAndroidPurchaseDidCompleteTransaction(transactionIDString, transactionReceiptString);

	env->ReleaseStringUTFChars(transactionID, tmpTransactionIDString);
	env->ReleaseStringUTFChars(transactionReceipt, tmpTransactionReceiptString);
}


JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onPurchaseFailedCallback(JNIEnv * env, jobject  obj, jstring transactionID, jstring transactionReceipt)
{
	const char *tmpTransactionIDString = env->GetStringUTFChars(transactionID, 0);
	const char *tmpTransactionReceiptString = env->GetStringUTFChars(transactionReceipt, 0);

	std::string transactionIDString = (tmpTransactionIDString);
	std::string transactionReceiptString = (tmpTransactionReceiptString);

	InAppPurchase::sharedInAppPurchase()->onAndroidPurchaseDidFailedTransaction(transactionIDString, transactionReceiptString);

	env->ReleaseStringUTFChars(transactionID, tmpTransactionIDString);
	env->ReleaseStringUTFChars(transactionReceipt, tmpTransactionReceiptString);
}


JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onPurchaseRestoreCallback(JNIEnv * env, jobject  obj, jstring transactionID, jstring transactionReceipt)
{
	const char *tmpTransactionIDString = env->GetStringUTFChars(transactionID, 0);
	const char *tmpTransactionReceiptString = env->GetStringUTFChars(transactionReceipt, 0);

	std::string transactionIDString = (tmpTransactionIDString);
	std::string transactionReceiptString = (tmpTransactionReceiptString);

	InAppPurchase::sharedInAppPurchase()->onAndroidPurchaseDidRestoreTransaction(transactionIDString, transactionReceiptString);

	env->ReleaseStringUTFChars(transactionID, tmpTransactionIDString);
	env->ReleaseStringUTFChars(transactionReceipt, tmpTransactionReceiptString);
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
