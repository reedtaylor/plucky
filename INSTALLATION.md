
# Instaling Plucky / DAYBREAK

- [Instaling Plucky / DAYBREAK](#instaling-plucky--daybreak)
- [What to Expect](#what-to-expect)
- [What you'll need](#what-youll-need)
- [Instructions](#instructions)
  - [Step 1: Prepare the DE1](#step-1-prepare-the-de1)
  - [Step 2: Remove the reservoir tube lifter knob](#step-2-remove-the-reservoir-tube-lifter-knob)
  - [Step 3: Remove the top-right back panel screw](#step-3-remove-the-top-right-back-panel-screw)
  - [Step 4: Loosen remaining back panel screws](#step-4-loosen-remaining-back-panel-screws)
  - [Step 5: Remove back panel](#step-5-remove-back-panel)
  - [Step 6: Remove Decent BLE adaptor](#step-6-remove-decent-ble-adaptor)
  - [Step 7: Install the DAYBREAK DE1 Interface daughterboard into DE1](#step-7-install-the-daybreak-de1-interface-daughterboard-into-de1)
  - [Step 8: Install Decent BLE adaptor into DAYBREAK](#step-8-install-decent-ble-adaptor-into-daybreak)
  - [Step 9: Route DAYBREAK wiring and position the back panel](#step-9-route-daybreak-wiring-and-position-the-back-panel)
  - [Step 10: Reattach the back panel](#step-10-reattach-the-back-panel)
  - [Step 11: Finish up!](#step-11-finish-up)
- [Final Notes:](#final-notes)
- [You are done!](#you-are-done)

Before we start, let's be real for a second:  

 * This project involves opening up your perfectly good multi-thousand-dollar espresso maker in order to perform a non-manufacturer-authorized upgrade, wherein you will install a piece of computer hardware made by some guy on the internet.
 * I, the aforemtioned internet guy, am neither a professional electrical engineer (anymore) nor a lawyer.  With that said, DAYBREAK and any related materials including sofware and documentation are provided "as is" and without any representation or warranty, expressed, implied, or otherwise, including any implied warranty of merchantability or of fitness for any particular purpose. 
 * In choosing install DAYBREAK, you yourself assume any and all risks involved.  These risks could include damage to your DE1, damage to your property, risks to life & limb, etc..
 * If the above gives you pause, please do not install DAYBREAK.

# What to Expect

It's very easy to install DAYBREAK into the back of your DE1!  It does not require specialized electronics or hardware skills.  I'd estimate it to be a 30 minute project, even if you work alone and take your time.

There are roughly four steps:

 * Removing 4 screws and a decorative lifty-knob
 * Carefully unplugging and replugging a small circuit board located on the back of the machine
 * (optional)  Attempting to perform step 9 below several times without a helper, which involves using one hand to awkwardly hold a short cable taut (but not TOO taut), one hand to maneuver the back panel of the machine into place, and a third hand to tighten a screw
 * Replacing the 4 screws and lifty-knob

When you are done, your machine will continue to work exactly as it always has.  The tablet will work as usual, and you won't even be able to DAYBREAK is there at all.  Your DE1 will simply have some new capabilities.

# What you'll need

Before starting, you should have the following things on hand:

1. A DAYBREAK Mk3b kit 
2. A Torx T10 wrench
3. Needle nose pliers (or some other suitable PCB/IC extractor tool)
4. A Micro USB cable
5. A USB power source.  Some choices:
   * If you plan to attach a USB host computer (Raspberry Pi, laptop, or desktop) to Plucky / DAYBREAK, this host computer will serve as the power source
   * For wifi-only operation, a wall-outlet USB power adaptor will work.  
   * Note: don't try to power DAYBREAK from the USB port on the front of the machine.  It won't work, for several resons.

# Instructions

## Step 1: Prepare the DE1 

Power down the DE1, and unhook EVERYTHING.  Unplug power from the wall, remove the power cord from the back of the DE1, and disconnect both the RJ45 and refill hoses, if attached.  Remove the drip tray and water reservoir.

<img src="https://user-images.githubusercontent.com/8826853/74622568-59224980-510f-11ea-8726-6dbee448f9a7.png" width=200>

## Step 2: Remove the reservoir tube lifter knob

Unscrew the knob and set aside.

<img src="https://user-images.githubusercontent.com/8826853/74622924-9fc47380-5110-11ea-9e1f-4cadf6784727.png" width=200>

## Step 3: Remove the top-right back panel screw 

Using a Torx T10 wrench, remove the top-right screw on the back of the machine, and set aside.

<img src="https://user-images.githubusercontent.com/8826853/74623374-5ffe8b80-5112-11ea-90a6-6dafdc8e0046.png" width=200>

## Step 4: Loosen remaining back panel screws

Use the Torx T10 wrench to release the remaining 3 screws on the back of the machine.  NOTE:  These do NOT need to be unscrewed fully from the decorative plastic cover, only enough to release the cover from the metal chassis, as shown.  (Installation may be easier if you leave these three screws engaged with the plastic cover rather than removing them fully.)

<img src="https://user-images.githubusercontent.com/8826853/74623440-a7851780-5112-11ea-8237-7953316de078.png" width=200> <img src="https://user-images.githubusercontent.com/8826853/74623264-f4b4b980-5111-11ea-8239-7b8c09adebb1.png" width=200>

## Step 5: Remove back panel

Remove the plastic panel with the 3 screws still protruding, and set aside.  You will now be looking at the back of the metal chassis, which looks like this: 

<img src="https://user-images.githubusercontent.com/8826853/74623533-ffbc1980-5112-11ea-9a9e-50d6a098bb11.png" width=200>

## Step 6: Remove Decent BLE adaptor

In this step we will remove the Decent BLE adaptor, which is the small printed circuit board (PCB) made accessible through an aperture in the metal chassis.  Yours may look slightly different, but the shape will be the same.

__We will exercise significant care in this step, to avoid damaging this PCB while removing it.__  Primarily, there are two things to be VERY careful about when removing this PCB: 

* We don't want to break any components off the surface of the PCB.  A few components are located very close to the edges, and could easily be damaged if we "slip" while trying to pull out the PCB.  __We will be careful to orient our tools so as to minimize the chances of damage, even if our tool "slips".__

   <img src="https://user-images.githubusercontent.com/8826853/74624161-62161980-5115-11ea-8bf2-1a808e9b0ca8.png" width=200>

   * We don't want to bend the pins of this PCB while pulling it out.  Any attempt to "rock" or "pry" it out, especially in a "top-to-bottom" orientation, is likely to bend the pins, so:  __We will be pulling straight out, not prying.__  

We probably aren't using professional-grade tools suited to this job, but a simple needle-nose pliers will do fine if used with a bit of care.  Alternatively, if you have a cheap IC extractor lying around from a soldering iron kit, today may be that tool's day to shine.

<img src="https://user-images.githubusercontent.com/8826853/74624574-f9c83780-5116-11ea-8215-4ce300d9dda9.png" width=200>

So: grasping the board firmly in the middle, with our tool in a __horizontal__ orientation, we will pull this board __straight__ out of the back of the machine.  The motion will resemble pulling a paper ticket from a dispenser: straight out.  Like this: 

<img src="https://user-images.githubusercontent.com/8826853/74624755-b6ba9400-5117-11ea-889b-8dc38da5dc42.gif" width=200>

## Step 7: Install the DAYBREAK DE1 Interface daughterboard into DE1
  
You may have noticed that the DE BLE adaptor looks very similar to the DAYBREAK daughterboard.  No accident there!

<img src="https://user-images.githubusercontent.com/8826853/74625192-7c51f680-5119-11ea-8b61-61ecd98dca43.png" width=200>

In this step we wil be replacing the board we just removed, with its DAYBREAK counterpart.  You may wish to place the DAYBREAK housing on top of the DE1 during this step.

<img src="https://user-images.githubusercontent.com/8826853/74625493-87f1ed00-511a-11ea-91fb-b930b45b2f8f.png" width=200>

The only thing tricky about this step is the fact that, like the BLE Adaptor, the DAYBREAK board has 8 pins, while the headers on the DE1 have only 6.  It is important to align these correctly: __The top 6 DAYBREAK pins will engage with the DE1, and the bottom two DAYBREAK pins be visibly not-connected.__

<img src="https://user-images.githubusercontent.com/8826853/74626001-42362400-511c-11ea-9241-3c62cde6a8cf.png" width=300>

## Step 8: Install Decent BLE adaptor into DAYBREAK

Now, assuming that you want to keep using the Decent BLE adaptor (so that your tablet keeps working), we will install that adaptor into its new home, on DAYBREAK.  Note that on DAYBREAK all 8 pins will be inserted, and the Decent logo should be toward the nearby edge of the case.

<img src="https://user-images.githubusercontent.com/8826853/74626706-6d217780-511e-11ea-95ea-1fdf9cfe4ce9.png" width=200>  <img src="https://user-images.githubusercontent.com/8826853/74626653-46634100-511e-11ea-85bf-0d691dddf6df.png" width=300>  

## Step 9: Route DAYBREAK wiring and position the back panel

In this step we will begin re-attaching the back panel.  

In order to keep the wiring neat, and to work within the limited length of our Picoblade ribbon cable, this step requires us to maintain a TINY bit of tension on the picoblade wires while maneuvering the back panel.  

Note: __This step is VERY easy if you have a helper to lend a hand__, but can certainly be done alone as well.  (The images will assume you are working alone.)

Hold the DAYBREAK housing straight off to the right of the DE1, as you face it from the rear.  Orient the DAYBREAK housing so that it faces away from you.  Look at the Picoblade wires and observe that they are not twisted, reorienting the DAYBREAK housing as needed to end up like this:

<img src="https://user-images.githubusercontent.com/8826853/74627343-8f1bf980-5120-11ea-91fb-ad8ebdff202b.png" width=200>

__If you have a helper, ask them to hold the box like this, and follow along as you complete the step.__

Lift the back panel into place and gently press it into position on the back of the machine. As you do this,  gently give more slack to the Picoblade wires, and bring the DAYBREAK housing around the edge to face you at the rear of the machine.

<img src="https://user-images.githubusercontent.com/8826853/74627799-ea9ab700-5121-11ea-9b2d-385c0153baa5.png">

There is enough tolerance for the thin wires to pass through without being pinched.  You will want the wires to end up like this:

<img src="https://user-images.githubusercontent.com/8826853/74627919-364d6080-5122-11ea-82f9-8ebd853a7a78.png" width=300>

## Step 10: Reattach the back panel 

Using the T10 Torx wrench, we will reattach the back panel.  As we complete this step, be careful not to over-tighten these screws as this could crack the plastic.

If working one-alone, you may need to do this with one hand (the other one will be holding the DAYBREAK housing in place).  To make this easier, start by using one of the still-attached screws that are NOT located at the top-right corner.  Tighten this only enough so that you no longer need to hold the back panel in place.

Then, secure the DAYBREAK housing at the top-right of the back panel, using the (fully removed) screw.  

Finally, secure all remaining screws.  Ensure these screws are engaging with the metal chassis and not just "pushing out" the plastic panel as you tighten them.  Also, 

<img src="https://user-images.githubusercontent.com/8826853/74627996-7280c100-5122-11ea-8a62-e032479a1727.png" width=200>

## Step 11: Finish up!


Reinstall the reservoir lifter tube knob.  Note, you may need to lift this tube from below, as the assembly drops a bit lower with the knob removed.

<img src="https://user-images.githubusercontent.com/8826853/74628602-161ea100-5124-11ea-8026-40010960345e.png" width=200>

Snap the cover onto the DAYBREAK housing, attach a USB cable, and that's it!  Put everything else back in place and you are good to go!

<img src="https://user-images.githubusercontent.com/8826853/74629178-9abdef00-5125-11ea-9d0d-5d5dae15a9bb.png" width=400>

    

# Final Notes:

* Your machine and tablet should operate normally, as if nothing has changed.  However,  __you must always provide USB power to DAYBREAK__, or the DE1 and BLE adaptor will not be able to communicate.  This will appear to the tablet as a lapse in communication, similar to a BLE dropout, and can cause mildly odd behaviors.  

* After you reattach and power everything on, if the machine does not go through its usual "5 ticks of awakening", follow these steps:
   1.  Power off DE1 (using switch on rear)
   2.  Remove power from DAYBREAK (unplug)
   3.  Power on DE1 (using switch at rear)
   4.  Wait 10 seconds
   5.  Restore power to DAYBREAK (plug in)

This avoids a rare but known condition where the DE1 can get confused during its power-up sequence by unexpected voltages coming from a partially-powered-up device, leading to a hang-up during initialization.  

Making sure the DE1 is the first device to receive power avoids this issue;  It usually doesn't seem to happen.

# You are done!  

Next steps will be covered in the confuiguration guide.





