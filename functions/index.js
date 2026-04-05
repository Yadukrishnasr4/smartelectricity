const functions = require('firebase-functions');
const admin = require('firebase-admin');
const twilio = require('twilio');

admin.initializeApp();

// Twilio Config (REPLACE WITH YOUR OWN)
const accountSid = 'YOUR_TWILIO_ACCOUNT_SID';
const authToken = 'YOUR_TWILIO_AUTH_TOKEN';
const twilioNumber = 'YOUR_TWILIO_PHONE_NUMBER';
const myNumber = 'YOUR_MOBILES_PHONE_NUMBER'; // The number to receive alerts

const client = new twilio(accountSid, authToken);

exports.monitorPower = functions.database.ref('/energy_monitoring/power')
    .onUpdate(async (change, context) => {
        const power = change.after.val();
        
        // Get threshold from database
        const thresholdSnap = await admin.database().ref('/energy_monitoring/threshold').once('value');
        const threshold = thresholdSnap.val() || 2000;

        if (power > threshold) {
            console.log(`Overload detected: ${power}W. Sending SMS...`);
            
            try {
                await client.messages.create({
                    body: `⚠️ Fusion Smart Alert: Overload Detected! Current usage is ${power}W. Load will be cut off.`,
                    to: myNumber,
                    from: twilioNumber
                });
                console.log('SMS sent successfully');
            } catch (error) {
                console.error('Error sending SMS:', error);
            }
        }
        return null;
    });
